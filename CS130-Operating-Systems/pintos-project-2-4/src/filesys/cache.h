#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include <list.h>
#include "devices/block.h"
#include "userprog/pagedir.h"
#include "threads/synch.h"
#include "filesys/inode.h"
#include "filesys/free-map.h"
#include "devices/block.h"
#include "off_t.h"

#define CACHE_SIZE 64
#define PERIOD 400
#define CACHE_NO_DATA_POS -1

struct read_ahead_sector
{
  block_sector_t sector_idx;
  struct list_elem elem;
};

struct filesys_cache 
{  
  block_sector_t sector_idx;
  uint8_t data[BLOCK_SECTOR_SIZE];
  struct lock lock;
  bool dirty;
  bool accessed;
};

struct lock l_access_cache;
struct list read_ahead_queue;
struct lock l_read_ahead;
struct condition read_ahead_cond;

void cache_init(void);
void writeback_to_disk(void);
void cache_read(block_sector_t sector_idx, void *buffer, off_t offset, off_t size);
void cache_write(block_sector_t sector_idx, const void *buffer, off_t offset, off_t size);
void read_ahead(block_sector_t sector_idx);

#endif
