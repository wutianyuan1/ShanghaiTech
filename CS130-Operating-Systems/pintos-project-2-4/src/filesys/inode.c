#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "threads/synch.h"

static const char zeros[BLOCK_SECTOR_SIZE];
static struct lock l_access_opennodes;
static void do_grow(int depth, int sector_num, block_sector_t* pos);
static void do_release(int depth, block_sector_t pos);
static bool do_inode_create (block_sector_t sector, off_t length, bool is_dir);

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return (size_t) DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* Grow the inode to length = LENGTH, DISKNODE is the inode
   on filesys disk. We use a DFS like algorithm to grow it 
   recursively, which is one of the most consise way. */
static void
inode_grow(struct inode_disk* disknode, off_t length)
{
  /* First, get the new length in term of sectors */
  block_sector_t new_len_sec = bytes_to_sectors(length);
  /* Then, decide how many sectors we need */
  int needed = min(new_len_sec, INDIRECT_SIZE);
  /* Grow the first level */
  do_grow(INDIRECT, needed, &disknode->idx_indirect);
  if (new_len_sec <= INDIRECT_SIZE)
    return;
  /* If doubly indirect is required, grow it again */
  new_len_sec -= INDIRECT_SIZE;
  do_grow(DOUBLE_INDIRECT, new_len_sec, &disknode->idx_dbl_indirect);
  return;
}

/* Release the contents of DISK_INODE
   Mark the bitmap as free, and also 
   do it recursively */
static void inode_release(struct inode_disk *disk_inode) 
{
  /* First, get length in term of sectors */
  int sector_num = bytes_to_sectors(disk_inode->length);
  /* Release first level (indirect) */
  do_release(INDIRECT, disk_inode->idx_indirect);
  if (sector_num <= INDIRECT_SIZE)
    return;
  /* Release the second level, doubly indirect */
  sector_num -= INDIRECT_SIZE;
  do_release(DOUBLE_INDIRECT, disk_inode->idx_dbl_indirect);
}

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t
byte_to_sector (const struct inode_disk *disk_inode, off_t pos)
{
  ASSERT (disk_inode != NULL);
  /* Calculate POS in term of sectors */
  off_t sector_ofs = pos / BLOCK_SECTOR_SIZE;
  /* If only indirect is allocated */
  if (sector_ofs < INDIRECT_SIZE)
  {
    /* Read indirect buffer, and find the sector index */
    block_sector_t* buffer = malloc(INDIRECT_SIZE*sizeof(block_sector_t));
    cache_read(disk_inode->idx_indirect, buffer, 0, BLOCK_SECTOR_SIZE);
    block_sector_t res = buffer[sector_ofs];    
    free(buffer);
    return res;
  }  
  /* If executes to here, doubly indirect is allocated. */
  block_sector_t* buffer = malloc(128*sizeof(block_sector_t));
  sector_ofs -= INDIRECT_SIZE;
  /* Find doubly indirect index and indirect index */
  off_t idx_dbl = sector_ofs / INDIRECT_SIZE;
  off_t idx_ind = sector_ofs % INDIRECT_SIZE;
  /* Get the result sector index */
  cache_read(disk_inode->idx_dbl_indirect, buffer, 0, BLOCK_SECTOR_SIZE);
  cache_read(buffer[idx_dbl], buffer, 0, BLOCK_SECTOR_SIZE);
  block_sector_t res = buffer[idx_ind];
  free(buffer);
  return res;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void
inode_init (void)
{
  list_init(&open_inodes);
  lock_init(&l_access_opennodes);
}

/* Create an inode, the type of it is FILE */
bool inode_create_file(block_sector_t sector, off_t length)
{
  return do_inode_create(sector, length, false);
}

/* Create an inode, the type of it is DIR */
bool inode_create_dir(block_sector_t sector, off_t length)
{
  return do_inode_create(sector, length, true);
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
  struct list_elem *e;
  struct inode *inode;

  lock_acquire(&l_access_opennodes);

  /* Check whether this inode is already open. */
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
     e = list_next (e))
  {
    inode = list_entry (e, struct inode, elem);
    if (inode->sector == sector)
    {
      lock_release(&l_access_opennodes);
      inode_reopen (inode);
      return inode;
    }
  }
  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL)
  {
    lock_release(&l_access_opennodes);
    return NULL;
  }
  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  lock_release(&l_access_opennodes);

  inode->sector = sector;
  inode->open_cnt = 1;
  inode->deny_write_cnt = 0;
  inode->removed = false;

  cache_read(inode->sector, &inode->data, 0, BLOCK_SECTOR_SIZE);
  inode->is_dir = inode->data.is_dir;
  return inode;
}

/* Reopens and returns INODE. */
struct inode*
inode_reopen (struct inode *inode)
{
  if (inode != NULL)
    inode->open_cnt++;
  return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
  return inode->sector;
}

/* Returns if the given INODE is a dir */
bool
inode_is_dir(const struct inode* inode)
{
  return inode->is_dir;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode)
{
  /* Ignore null pointer. */
  if (inode == NULL)
    return;
  /* Release resources if this was the last opener. */
  if(--inode->open_cnt == 0)
  {
    lock_acquire(&l_access_opennodes);
    list_remove(&inode->elem);
    lock_release(&l_access_opennodes);
    /* Deallocate blocks if removed. */
    if (inode->removed)
    {
      free_map_release (inode->sector, 1);
      inode_release(&inode->data);
    }
    free (inode);
  }    
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode)
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset)
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  struct inode_disk *disk_inode = &inode->data;
  ASSERT(disk_inode->magic == INODE_MAGIC);

  while (size > 0)
    {
      /* Disk sector to read, starting byte offset within sector. */    
      block_sector_t sector_idx = byte_to_sector (disk_inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length(inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      /* read from cache directly, modified by madoka */
      if (disk_inode->length - offset > BLOCK_SECTOR_SIZE)
        read_ahead(byte_to_sector(disk_inode, offset + BLOCK_SECTOR_SIZE));
      cache_read(sector_idx, buffer + bytes_read, sector_ofs, chunk_size);

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size, off_t offset)
{
  struct inode_disk *disk_inode = &inode->data;
  ASSERT(disk_inode->magic == INODE_MAGIC);
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;
  bool growed = false;
  if (inode->deny_write_cnt)
    return 0;

  /* To find if growth is needed */
  if (offset + size > disk_inode->length)
	{
    inode_grow(&inode->data, offset + size);
    growed = true;
  }
  while (size > 0)
  {
    /* Sector to write, starting byte offset within sector. */    
    block_sector_t sector_idx = byte_to_sector (disk_inode, offset);
    int sector_ofs = offset % BLOCK_SECTOR_SIZE;

    int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;

    /* Number of bytes to actually write into this sector. */
    int chunk_size = size < sector_left ? size : sector_left;
    if (chunk_size <= 0)
      break;

		/* write from cache directly, modified by madoka */
    cache_write(sector_idx, buffer + bytes_written, sector_ofs, chunk_size);

    /* Advance. */
    size -= chunk_size;
    offset += chunk_size;
    bytes_written += chunk_size;
    if (disk_inode->length < offset)
      disk_inode->length = offset;
  }
  /* If growed the INODE, we need to write it back */
  if (growed)
    cache_write(inode->sector, &inode->data, 0, BLOCK_SECTOR_SIZE);
  return bytes_written;
}


/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
static bool
do_inode_create (block_sector_t sector, off_t length, bool is_dir)
{
  struct inode_disk *disk_inode = NULL;

  ASSERT (length >= 0);

  /* If this assertion fails, the inode structure is not exactly
     one sector in size, and you should fix that. */
  ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

  disk_inode = calloc(1, sizeof *disk_inode);
  if (!disk_inode)
    return false;
  /* Initialize */
  disk_inode->is_dir = is_dir;
  disk_inode->length = length;
  disk_inode->magic = INODE_MAGIC;
  inode_grow(disk_inode, length);
  cache_write(sector, disk_inode, 0, BLOCK_SECTOR_SIZE);
  free(disk_inode);
  return true;
}

/* Do INODE growth, to depth = DEPTH
   Use a DFS like algorithm, Madoka think
   that's COOL! */
static void
do_grow(int depth, int sector_num, block_sector_t* pos)
{
  if (*pos == 0)
  {
    /* If not exists, create a new sector */
    free_map_allocate(1, pos);
    cache_write(*pos, zeros, 0, BLOCK_SECTOR_SIZE);
  }
  /* Recursion end condition */
  if (depth == DIRECT)
    return;
  /* Read from the disk */
  int buffer[INDIRECT_SIZE];
  cache_read(*pos, buffer, 0, BLOCK_SECTOR_SIZE);
  int per_sec_size = 1, current = 0;
  if (depth == DOUBLE_INDIRECT)
    per_sec_size = INDIRECT_SIZE;
  /* Find the last un-free sector */
  for (; current < DIV_ROUND_UP(sector_num, per_sec_size); current ++)
  {
    if (buffer[current] == 0)
      break;    
  }
  /* Current is the sector need to start to be modified */
  if (current != 0)
    current --;
  /* Grow it! If depth != 0, grow recursively */
  for (; current < DIV_ROUND_UP(sector_num, per_sec_size); current ++)
  {
    do_grow(depth - 1, per_sec_size, &buffer[current]);
  }
  /* If modified, write it back to disk */
  cache_write(*pos, buffer, 0, BLOCK_SECTOR_SIZE);
}

/* Do INODE release, to depth = DEPTH
   Use a DFS like algorithm, Madoka think
   that's COOL! */
static void
do_release(int depth, block_sector_t pos)
{
  /* Release it in free-map, recursion end */
  if (depth == 0)
  {
    free_map_release(pos, 1);
    return;
  }
  /* Read the buffer from disk */
  int buffer[INDIRECT_SIZE];
  cache_read(pos, buffer, 0, BLOCK_SECTOR_SIZE);
  /* Release recursively */
  for (int i = 0; i < INDIRECT_SIZE && buffer[i] != 0; i++)
    do_release(depth - 1, buffer[i]);
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode)
{
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode)
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  return inode->data.length;
}
