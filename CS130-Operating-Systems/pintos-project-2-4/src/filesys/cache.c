#include "filesys/cache.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "devices/block.h"
#include "devices/timer.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "filesys.h"

/* All cache lines, total 64*512bytes */
static struct filesys_cache* caches[CACHE_SIZE];

static struct filesys_cache* find_cache(block_sector_t sector);
static int  get_free_idx(void);
static void periodic_write_func(void *aux UNUSED);
static void read_ahead_func(void *aux UNUSED);
static void single_cache_init(struct filesys_cache* _cache);

/* Initialize a single cacheline, set ACCESSED and DIRTY to False */
static void
single_cache_init(struct filesys_cache* _cache)
{
  /* Initialize the lock and member varibles */
  lock_init(&_cache->lock);
  _cache->accessed = false;
  _cache->dirty = false;
  /* No sector correspond to it */
  _cache->sector_idx = CACHE_NO_DATA_POS;
}

/* Initialize all cache system, create threads, allocate cache memory */
void
cache_init() 
{
  /* Alloc memory and initialize all cachelines */
  for (int i = 0; i < CACHE_SIZE; ++i)
  {
    caches[i] = malloc(sizeof(struct filesys_cache));
    single_cache_init(caches[i]);
  }
  /* Initialize global varibles */
  lock_init(&l_access_cache);
  list_init(&read_ahead_queue);
  lock_init(&l_read_ahead);
  cond_init(&read_ahead_cond);
  /* create writter adn reader threads */
  thread_create("periodic_writter", PRI_DEFAULT, periodic_write_func, NULL);
  thread_create("read_ahead_reader", PRI_DEFAULT, read_ahead_func, NULL);
}

/* Give a sector, find if it is in cache */
static struct filesys_cache*
find_cache(block_sector_t sector)
{
  /* Traverse the cacheline, to find it */
  for (int i = 0; i < CACHE_SIZE; ++ i)
  {
    lock_acquire(&caches[i]->lock);
    /* If found, return it */
    if (caches[i]->sector_idx == sector)
      return caches[i];
    lock_release(&caches[i]->lock);
  }
  return NULL;
}

/* Read the content corresponds to SECTOR_IDX to buffer,
   At OFFSET, read size = SIZE */
void
cache_read(block_sector_t sector_idx, void *buffer, off_t offset, off_t size)
{
  lock_acquire(&l_access_cache);
  /* First, find if it's in cache */
  struct filesys_cache* ret = find_cache(sector_idx);
  /* If not in cache, get a free cacheline, and read from disk */
  if (!ret)
  {
    int free_idx = get_free_idx();    
    caches[free_idx]->sector_idx = sector_idx;
    block_read(fs_device, sector_idx, caches[free_idx]->data);    
    ret = caches[free_idx];
  }  
  /* Copy contents from cache to buffer */
  memcpy(buffer, ret->data + offset, size);
  ret->accessed = true;
  /* For synchronization */
  lock_release(&ret->lock);
  lock_release(&l_access_cache);
}

/* Write the buffer to disk corresponds to SECTOR_IDX,
   At OFFSET, read size = SIZE */
void
cache_write(block_sector_t sector_idx, const void *buffer, off_t offset, off_t size)
{
  lock_acquire(&l_access_cache);
  /* First, find if it's in cache */
  struct filesys_cache* ret = find_cache(sector_idx);
  /* If not in cache, get a free cacheline, and read from disk */
  if (!ret)
  {
    int free_idx = get_free_idx();
    caches[free_idx]->sector_idx = sector_idx;
    block_read(fs_device, sector_idx, caches[free_idx]->data);    
    ret = caches[free_idx];
  }  
  /* Copy contents from buffer to cache */
  memcpy(ret->data + offset, buffer, size);
  ret->accessed = true;
  ret->dirty = true;
  /* For synchronization */
  lock_release(&ret->lock);
  lock_release(&l_access_cache);
}

/* Get a free cacheline index, there are 2 possible cases:
   (1) Cache not full, just find the first empty one
   (2) Cache is full, need evict a cache to disk and return it. */
static int
get_free_idx()
{
  int i = 0;
  while (1)
  {
    /* Find a not in use cache */
    if (lock_try_acquire(&caches[i]->lock))
    {
      /* If it's accessed, set it to False */
      if (caches[i]->accessed)
        caches[i]->accessed = false;
      /* Get first non-accessed cache */
      else
      {
        /* If it is dirty, write it back to disk */
        if (caches[i]->dirty)
          block_write(fs_device, caches[i]->sector_idx, caches[i]->data);
        /* Get it, return index */
        return i;
      }
      lock_release(&caches[i]->lock);
    }
    /* Reset the iterator */
    if (++i == CACHE_SIZE)
      i = 0;
  }
}

/* Write all caches to disk */
void 
writeback_to_disk(void)
{
  /* Traverse the cacheline */
  for (int i = 0; i < CACHE_SIZE; i++)
  {
    lock_acquire(&caches[i]->lock);
    /* Dirty caches should be writeback to disk */
    if (caches[i]->dirty)
    {
      caches[i]->dirty = false;
      block_write(fs_device, caches[i]->sector_idx, caches[i]->data);
    }
    /* For synchronization */
    lock_release(&caches[i]->lock);
  }
}

/* Write the data back to disk periodly */
static void
periodic_write_func(void *aux UNUSED)
{
  while (true) 
  {  
    /* Flush the buffer every PERIOD time */
    timer_sleep(PERIOD);
    writeback_to_disk();
  }
}

/* Read ahead thread's function. pop from the queue
   and read the contents into cache */
static void
read_ahead_func(void *aux UNUSED)
{
  while (1)
  {    
    /* Wait until queue is not empty */
    int buffer[128];
    lock_acquire(&l_read_ahead);
    while (list_empty(&read_ahead_queue))
      cond_wait(&read_ahead_cond, &l_read_ahead);
    /* Get the first element */
    struct read_ahead_sector *read_sec = list_entry(list_begin(&read_ahead_queue),
                                                    struct read_ahead_sector, elem);
    lock_release(&l_read_ahead);
    block_sector_t sector_idx = read_sec->sector_idx;
    /* Delete the element and free memory */
    list_remove(&read_sec->elem);
    free(read_sec);    
    /* Read it into cache */  
    cache_read(sector_idx, buffer, 0, BLOCK_SECTOR_SIZE);
  }
}

/* Read ahead API, called by "inode_read_at"
   Push the need-read sector into queue */
void
read_ahead(block_sector_t sector_idx)
{
  /* Alloc a element and push the sector into queue */
  lock_acquire(&l_read_ahead);
  struct read_ahead_sector* read_sec = malloc(sizeof(struct read_ahead_sector));
  read_sec->sector_idx = sector_idx;
  list_push_back(&read_ahead_queue, &read_sec->elem);
  /* Give a signal to reader */
  cond_signal(&read_ahead_cond, &l_read_ahead);
  lock_release(&l_read_ahead);
}
