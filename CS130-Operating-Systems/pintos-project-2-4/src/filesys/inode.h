#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include <stdbool.h>
#include <bitmap.h>
#include "devices/block.h"
#include "filesys/off_t.h"
#include "threads/synch.h"


/* Identifies an inode. */
#define INODE_MAGIC       0x494e4f44
#define DIRECT            0
#define INDIRECT          1
#define DOUBLE_INDIRECT   2
#define INDIRECT_SIZE     (BLOCK_SECTOR_SIZE / 4)
#define min(a, b)         ((a) < (b) ? (a) : (b))

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
{
  block_sector_t idx_indirect;
  block_sector_t idx_dbl_indirect;
  unsigned is_dir;                       /* Is a directory or not. */
  off_t length;                          /* File size in bytes. */
  unsigned magic;                        /* Magic number. */
  int unused[123];
};

/* In-memory inode. */
struct inode
{
  struct list_elem elem;                /* Element in inode list. */
  struct lock l_access_inode;           /* Lock */
  block_sector_t sector;                /* Sector number of disk location. */
  int open_cnt;                         /* Number of openers. */
  bool removed;                         /* True if deleted, false otherwise. */
  bool is_dir;
  block_sector_t parent_sec;
  int deny_write_cnt;                   /* 0: writes ok, >0: deny writes. */
  struct inode_disk data;               /* Inode content. */
};

void inode_init (void);
/* Modified by madoka */
bool inode_create_file (block_sector_t, off_t);
bool inode_create_dir  (block_sector_t, off_t);
bool inode_is_dir (const struct inode*);

struct inode *inode_open (block_sector_t);
struct inode *inode_reopen (struct inode *);
block_sector_t inode_get_inumber (const struct inode *);
void inode_close (struct inode *);
void inode_remove (struct inode *);
off_t inode_read_at (struct inode *, void *, off_t size, off_t offset);
off_t inode_write_at (struct inode *, const void *, off_t size, off_t offset);
void inode_deny_write (struct inode *);
void inode_allow_write (struct inode *);
off_t inode_length (const struct inode *);

#endif /* filesys/inode.h */
