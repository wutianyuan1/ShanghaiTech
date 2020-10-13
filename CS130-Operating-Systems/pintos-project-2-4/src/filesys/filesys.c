#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "filesys/cache.h"
#include "threads/thread.h"
#include "userprog/syscall.h"
#include "threads/malloc.h"

/* Partition that contains the file system. */
struct block *fs_device;
static void do_format (void);

/* Get current thread's parent's inode index */
static block_sector_t thread_get_parent_idx(void);

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void
filesys_init (bool format) 
{
  fs_device = block_get_role (BLOCK_FILESYS);
  if (fs_device == NULL)
    PANIC ("No file system device found, can't initialize file system.");
  /* Initialize buffer cache, free map and inode */
  inode_init ();
  free_map_init ();
  cache_init();
  /* Format */
  if (format) 
    do_format ();
  free_map_open ();
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void
filesys_done (void) 
{
  writeback_to_disk();
  free_map_close ();
}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create (const char *name, off_t initial_size) 
{
  block_sector_t inode_sector = 0;
  /* Parse the full name, to get dir and filename */
  struct path_parse_ret* ret = parse_path((char*)name);
  if (!ret->parse_success)
    return false;
  bool success = (ret->dir != NULL
                  && free_map_allocate (1, &inode_sector)
                  && inode_create_file (inode_sector, initial_size)
                  && dir_add (ret->dir, ret->filename, inode_sector, false));
  if (!success && inode_sector != 0) 
    free_map_release (inode_sector, 1);
  dir_close (ret->dir);
  return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *
filesys_open (const char *name)
{
  /* Parse the full name, to get dir and filename */
  struct path_parse_ret* ret = parse_path((char*)name);
  if (!ret->parse_success)
    return false;
  /* If want to open root, just open ROOT DIR */
  struct inode *inode = NULL;
  if (ret->is_root && ret->is_dir)
    return file_open(inode_open(ROOT_DIR_SECTOR));
  /* Lookupo and open it */
  if (ret->dir != NULL)
    dir_lookup (ret->dir, ret->filename, &inode);
  dir_close (ret->dir);
  return file_open (inode);
}


/* To get current thread's parent dir's inode sectoe index */
static block_sector_t
thread_get_parent_idx()
{
  struct inode* inode;
  /* Root's parent is still ROOT */
  if (thread_current()->cwd->inode->sector == ROOT_DIR_SECTOR)
    return ROOT_DIR_SECTOR;
  /* Find ".." and get it inode number */
  dir_lookup(thread_current()->cwd, "..", &inode);
  inode_close(inode);
  /* Return parent inode sector index */
  return inode->sector;
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool
filesys_remove (const char *name) 
{
  struct path_parse_ret* ret = parse_path((char*)name);
  if (!ret->parse_success)
    return false;
  /* Checking for rm cwd and rm parent */
  if (ret->dir != NULL)
  {
    struct inode* inode;
    dir_lookup(ret->dir, ret->filename, &inode);
    /* If you want to remove a dir, must check if it's legal */
    if (inode->is_dir)
    {
      /* Get cwd inode sector index and parent cwd index */
      block_sector_t cwd_secidx = thread_current()->cwd->inode->sector;
      block_sector_t par_secidx = thread_get_parent_idx();
      /* If illegal cases occur, close and return false */ 
      if (inode->sector == cwd_secidx || inode->sector == par_secidx || inode->open_cnt > 6)
      {
        dir_close(ret->dir);        
        return false;
      }
    }  
  }
  /* remove it, and return whether remove success */ 
  bool success = (ret->dir != NULL) && dir_remove (ret->dir, ret->filename);
  dir_close (ret->dir); 
  return success;
}

/* Formats the file system. */
static void
do_format (void)
{
  printf ("Formatting file system...");
  free_map_create ();
  if (!dir_create (ROOT_DIR_SECTOR, 16))
    PANIC ("root directory creation failed");
  free_map_close ();
  printf ("done.\n");
}
