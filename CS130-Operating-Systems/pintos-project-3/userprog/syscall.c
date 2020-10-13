#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include <list.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"
#include "threads/malloc.h"

static void syscall_handler (struct intr_frame *);

/**
 * Add this lock to do synchronizing
 * for only one thread can access file system
 * if there it a thread do that, other threads must wait
 */
struct lock l_access_filesys;


/************************ fd manager ***********************************/

/**
 * An array stores files
 * which can find fd by given filename
 * The i th file has descriptor 2
 */
struct file_and_fd
{
  struct file* _file;     /* file pointer */ 
  int fd;                 /* file descriptor */
  int holder_tid;         /* holder's tid */
  char* filename;         /* filename */
  struct list_elem elem;  /* list element */
};

struct file_manager
{
  struct list l;          /* file list */
  int max_fd;             /* current maximum fd */
};  

/* All opened files */
static struct file_manager curr_files;
/* initalize file manager */
static void file_manager_init(struct file_manager* fm);
/* returns a file-descriptor */
static int add_file(struct file_manager* fm, struct file* f, char* fn);
/* get a struct file by a given fd */
static struct file* get_file(struct file_manager* fm, int fd);
/* remove a given file in manager */
static void remove_file(struct file_manager* fm, struct file* f);
/* get a file's name by given fd */
static char* get_name(struct file_manager* fm, int fd);
/* get a file's holder thread tid by given fd */
static int get_holder_tid(struct file_manager* fm, struct file* f);
/* get 1 byte user's memory */
static int get_user (const uint8_t *uaddr);
/* check if the given filename is valid */
bool check_file(char* filename, struct intr_frame* f);
/* close all file opened by current thread */
void filesys_close_all(tid_t tid);

/* initalize file manager */
static void 
file_manager_init(struct file_manager* fm)
{
  /* stdin, stdout, stderr is pre-served */
  fm->max_fd = 2;
  /* init file list */
  list_init(&fm->l);
}

/* returns a file-descriptor */
static int 
add_file(struct file_manager* fm, struct file* f, char* fn)
{
  /* fd starts from 2 */
  struct file_and_fd* new_file = malloc(sizeof(struct file_and_fd));
  if (new_file == NULL)
    return -1;
  /* allocate a new fd */
  fm->max_fd++;
  /* set file, name and fd */
  new_file->_file = f;
  new_file->fd = fm->max_fd;
  new_file->filename = fn;
  /* push into list */
  list_push_back(&fm->l, &new_file->elem);
  /* return allocted fd */
  return fm->max_fd;
}

/* get a struct file by a given fd */
static struct file*
get_file(struct file_manager* fm, int fd)
{
  /* traverse file list to get pointer to file */
  for (struct list_elem* iter = list_begin(&fm->l);
       iter != list_end(&fm->l);
       iter = list_next(iter))
  {
    /* if matches, return this file */
    struct file_and_fd* curr = list_entry(iter, struct file_and_fd, elem);
    if (curr->fd == fd)
      return curr->_file;
  }
  /* NULL for not found */
  return NULL;
}

/* get a file's name by given fd */
static char*
get_name(struct file_manager* fm, int fd)
{
  /* traverse file list to get pointer to filename */
  for (struct list_elem* iter = list_begin(&fm->l);
       iter != list_end(&fm->l);
       iter = list_next(iter))
  {
    struct file_and_fd* curr = list_entry(iter, struct file_and_fd, elem);
    /* if matches, return this file */
    if (curr->fd == fd)
      return curr->filename;
  }
  /* NULL for not found */
  return NULL;
}

/* remove a given file in manager */
static void 
remove_file(struct file_manager* fm, struct file* f)
{
  /* traverse file list to get pointer to file need to remove */
  for (struct list_elem* iter = list_begin(&fm->l);
       iter != list_end(&fm->l);
       iter = list_next(iter))
  {
    struct file_and_fd* curr = list_entry(iter, struct file_and_fd, elem);
    /* if matches, remove this file */
    if (curr->_file == f)
    {
      list_remove(iter);
      free(curr);
      /* after removing, returns */
      return;
    }
  }
}

/* get a file's holder thread tid by given fd */
static int 
get_holder_tid(struct file_manager* fm, struct file* f)
{
  /* traverse file list to get matching thread's tid */
  for (struct list_elem* iter = list_begin(&fm->l);
       iter != list_end(&fm->l);
       iter = list_next(iter))
  {
    struct file_and_fd* curr = list_entry(iter, struct file_and_fd, elem);
    /* if matches, return the holder's tid*/
    if (curr->_file == f)
      return curr->holder_tid;
  }
  /* -1 for not found */
  return -1;
}


/******************** Given 2 functions ***********************/
/**
 * Copied on cs140 website, DONOT modify it
 * I just use it to check if the memory is valid
 * If you abuse it, madoka will get annoied.
 */

/* get 1 byte user's memory */
static int
get_user (const uint8_t *uaddr)
{
  /* check addr is user's */
  if(!is_user_vaddr(uaddr))
    return -1;
  int result;
  /* get result */
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* The initalization of syscall. */
void
syscall_init (void) 
{
  /* initalize filesys access lock */
  lock_init(&l_access_filesys);
  /* 0x30 is syscall */
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  /* initalize file manager */
  file_manager_init(&curr_files);
}

/****************** Filesy helpers  ***************************/
/**
 * check if the given filename can be open
 * return 1 if success, otherwise, 0 for failed
 */
int filesys_try_open(char * filename)
{
  /* try to open it */
  struct file* f = filesys_open(filename);
  if (f)
  {
    /* open success, should close it */
    file_close(f);
    /* return success */
    return 1;
  }
  /* return fail */
  return 0;
}

/* close all file opened by current thread */
void 
filesys_close_all(tid_t tid)
{
  struct list_elem* iter = list_begin(&curr_files.l);
  /* traverse the list to find the matching file */
  while (iter != list_end(&curr_files.l))
  {
    struct file_and_fd* curr = list_entry(iter, struct file_and_fd, elem);
    /* if match, close and remove it from list */
    if (curr->holder_tid == tid)
    {
      file_close(curr->_file);
      iter = list_remove(iter);
      /* free the memory */
      free(curr);
    }
    /* not match, continue */
    else
      iter = list_next(iter);
  }
}

/* check if the given filename is valid */
bool 
check_file(char* filename, struct intr_frame* f)
{
  /* check if the filename is a valid memory address*/
  if (filename == NULL 
   || get_user((const uint8_t*)filename) == -1)
  {
    /* if not valid, exit -1 */
    *(int *)(f->esp + 4) = -1;
    syscall_exit_1(f);
    NOT_REACHED ();
  }
  /* check filename's length not >=14 or <=0 */
  if (strlen(filename) >= 14 
   || strlen(filename) <= 0
   || strcmp(filename, "") == 0 )
  {
    /* if not valid, do nothing and return */
    return false;
  }
  return true;
}

/******************* Syscalls 0-12 ****************************/

/* syscall: SYS_SHUTDOWN = 0 */
void 
syscall_shutdown_0(struct intr_frame *f UNUSED)
{
  /* shutdown & power off */
  shutdown_power_off();
}

/* syscall: SYS_EXIT = 1 */
void
syscall_exit_1(struct intr_frame *f)
{
  /* get return value */
  int ret = *(int *)(f->esp + 4);
  /* set current's retval to this value */
  struct thread* curr = thread_current();
  curr->retval = ret;
  /* exit thread */
  thread_exit();
}

/* syscall: SYS_EXEC = 2 */
void 
syscall_exec_2(struct intr_frame *f)
{
  /* get executable filename */
  char *filename  = *(char**)(f->esp + 4);
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* get executable cmd name */
  char *save_ptr;  
  char temp[64];
  strlcpy(temp, filename, 64);
  char* fn = strtok_r (temp, " ", &save_ptr);
  /* try to open it, verify it exists */
  if (filesys_try_open(fn))
    f->eax = process_execute(filename);
  else
    f->eax = -1;
   /* filesys synchronization */
  lock_release(&l_access_filesys);
}

/* syscall: SYS_WAIT = 3 */
void 
syscall_wait_3(struct intr_frame *f)
{
  /* wait this pid and returns */
  unsigned pid = *(uint32_t*)(f->esp + 4);
  f->eax = process_wait(pid);
}

/* syscall: SYS_CREATE = 4 */
void 
syscall_create_4(struct intr_frame *f)
{
  /* get filename */
  char *filename  = *(char**)(f->esp + 4);
  /* get size */
  unsigned size = *(uint32_t*)(f->esp + 8);
  /* check the given filename */
  if (!check_file(filename, f))
  {
    /* if not valid, do nothing and return */
    f->eax = 0;
    return;
  }
  else
  {
    /* filesys synchronization */
    lock_acquire(&l_access_filesys);
    /* create it in filesys, if failed, return 0 */
    if (!filesys_create(filename, size))
    {
      f->eax = 0;
      /* release lock */
      lock_release(&l_access_filesys);
      return;
    }
    /* success, return 1 */
    f->eax = 1;
    /* filesys synchronization */
    lock_release(&l_access_filesys);
    return;
  }
}

/* syscall: SYS_REMOVE = 5 */
void
syscall_remove_5(struct intr_frame *f)
{
  /* get filename */
  char *filename  = *(char**)(f->esp + 4);
  /* check the given filename */
  if (!check_file(filename, f))
  {
    /* if not valid, do nothing and return */
    f->eax = 0;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* failed to remove */
  if (!filesys_remove(filename))
  {
    f->eax = 0;
    /* release lock */
    lock_release(&l_access_filesys);
    return;
  }
  f->eax = 1;
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_OPEN = 6 */
void
syscall_open_6(struct intr_frame *f)
{
  /* get filename */
  char *filename  = *(char**)(f->esp + 4);
  /* check the given filename */
  if (!check_file(filename, f))
  {
    /* if not valid, do nothing and return */
    f->eax = -1;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* open in filesys */
  struct file* openfile = filesys_open(filename);
  /* not success */
  if (openfile == NULL)
  {
    f->eax = -1;
    /* release lock */
    lock_release(&l_access_filesys);
    return;
  }
  /* add a file to list and returns a file-descriptor */
  f->eax = add_file(&curr_files, openfile, filename);
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_FILESIZE = 7 */
void
syscall_filesize_7(struct intr_frame* f)
{
  /* get fd number */
  int fd  = *(int *)(f->esp + 4);
  int max_fd = curr_files.max_fd;  
  /* invalid file descriptor */
  if (fd > max_fd || fd == 1 || fd < 0)
  {
    /* do nothing and return */
    f->eax = 0;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* get matching file */
  struct file* _file = get_file(&curr_files, fd);
  /* no such file */
  if (!_file)
  {
    /* return -1 */
    f->eax = -1;
    lock_release(&l_access_filesys);
    return;
  }
  /* get filesize */
  int filesize = file_length(_file);
  f->eax = filesize;
  /* filesys synchronization */
  lock_release(&l_access_filesys);
}

/* syscall: SYS_READ = 8 */
void 
syscall_read_8(struct intr_frame* f)
{
  int read_fd   = *(int *)(f->esp + 4);          /* file descriptor */
  char *buffer  = *(char**)(f->esp + 8);         /* buffer */
  unsigned size = *(uint32_t*)(f->esp + 12);     /* read size */
  int max_fd = curr_files.max_fd; 
  /* invalid file descriptor */
  if (read_fd > max_fd || read_fd == 1 || read_fd < 0)
  {
    f->eax = 0;
    return;
  }
  /* invalid buffer address */
  if ( buffer == NULL
    || get_user((const uint8_t*)(buffer + size - 1)) == -1
    || get_user((const uint8_t*)buffer) == -1)
  {
    /* return -1 */
    *(int *)(f->esp + 4) = -1;
    syscall_exit_1(f);
  }
  /* read stdin */
  if (read_fd == 0)
  {
    unsigned readsz;
    /* read byte by byte... QwQ */
    for (readsz = 0; readsz < size; readsz++)
      *(buffer+readsz) = input_getc();
    /* return size */
    f->eax = readsz;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  struct file* readfile = get_file(&curr_files, read_fd);
  /* no such file */
  if (readfile == NULL)
  {
    f->eax = -1;
    lock_release(&l_access_filesys);
    return;
  }
  /* read from filesys */
  int readsize = file_read(readfile, buffer, size);
  f->eax = readsize;
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_WRITE = 9 */
void
syscall_write_9(struct intr_frame *f)
{
  /* get arguments */
  int write_fd  = *(int *)(f->esp + 4);       /* write to where? */
  char *buffer  = *(char**)(f->esp + 8);      /* contents buffer */  
  unsigned size = *(uint32_t*)(f->esp + 12);  /* write size */
  int max_fd = curr_files.max_fd;             /* check fd is valid */
  /* check memory address all valid */
  if ( buffer == NULL
    || get_user((const uint8_t*)buffer + size - 1) == -1
    || get_user((const uint8_t*)buffer) == -1)  
  {
    /* for invalid cases, exit -1 */
    *(int *)(f->esp + 4) = -1;
    syscall_exit_1(f);
  }
  /* invalid fd */
  if (write_fd > max_fd || write_fd <= 0)
  {
    /* write nothing and return */
    f->eax = 0;
    return;
  }
  /* write to stdout */
  if (write_fd == STDOUT_FILENO)
  {
    /* Print it directly */
    putbuf(buffer, size);
    /* return write size */
    f->eax = size;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* get write filename by given fd */
  struct file* writefile = get_file(&curr_files, write_fd);
  /* check if write to a executable file */
  char* writename = get_name(&curr_files, write_fd);    /* write to what? */
  /* To executes! forbidden! */
  if (!can_write(writename))
    f->eax = 0;
  /* normal */
  else
    f->eax = file_write(writefile, buffer, size);
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_SEEK = 10 */
void
syscall_seek_10(struct intr_frame *f)
{
  /* file descriptor, position*/
  int seek_fd  = *(int *)(f->esp + 4);
  int pos = *(uint32_t*)(f->esp + 8);
  int max_fd = curr_files.max_fd; 
  /* invalid file descriptor */
  if (seek_fd > max_fd || seek_fd <= 2)
  {
    f->eax = 0;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  struct file* seekfile = get_file(&curr_files, seek_fd);  
  /* invalid seekfile */
  if (seekfile == NULL
   || pos >= file_length(seekfile))
  {
    lock_release(&l_access_filesys);
    return;
  }
  /* do seek */
  file_seek(seekfile, pos); 
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_TELL = 11 */
void
syscall_tell_11(struct intr_frame *f)
{
  /* tell file descriptor */
  int tell_fd  = *(int *)(f->esp + 4);
  int max_fd = curr_files.max_fd; 
  /* invalid file descriptor */
  if (tell_fd > max_fd || tell_fd <= 2)
  {
    f->eax = 0;
    return;
  }
  /* filesys synchronization */
  lock_acquire(&l_access_filesys);
  /* get file */
  struct file* tellfile = get_file(&curr_files, tell_fd);
  /* no such file */
  if (tellfile == NULL)
  {
    /* release lock */
    lock_release(&l_access_filesys);
    /* do nothing and return */
    f->eax = 0;
    return;  
  }  
  /* do tell and return */
  f->eax = file_tell(tellfile);
  /* filesys synchronization */
  lock_release(&l_access_filesys);
  return;
}

/* syscall: SYS_CLOSE = 12 */
void 
syscall_close_12(struct intr_frame *f)
{
  /* get fd */
  int close_fd  = *(int *)(f->esp + 4);
  /* invalid file descriptor */
  if (close_fd < 3 || close_fd > curr_files.max_fd)
    return;
  /* get closefile */
  struct file* closefile = get_file(&curr_files, close_fd);
  /* close file not belongs to it, or close NULL */
  if (closefile == NULL
   || get_holder_tid(&curr_files, closefile) != thread_current()->tid)
    return;
  /* close this file */
  file_close(closefile);
  /* remove it from list */
  remove_file(&curr_files, closefile);
}

/* syscall handler, handle all syscalls */
static void
syscall_handler (struct intr_frame *f) 
{
  /* get syscall number */
  int syscall_num = * (int *)f->esp;
  /* invalid memory address handler */
  if (!is_user_vaddr(f->esp + 4) || f->esp == NULL)
  {
    *(int *)(f->esp + 4) = -1;
    syscall_exit_1(f);
  }
  /* switch syscall_num */
  switch (syscall_num)
  {
    case SYS_HALT:
      syscall_shutdown_0(f);
      break;
    case SYS_EXIT:
      syscall_exit_1(f);
      break;
    case SYS_EXEC:
      syscall_exec_2(f);
      return;
    case SYS_WAIT:
      syscall_wait_3(f);
      return;
    case SYS_CREATE:
      syscall_create_4(f);
      break;
    case SYS_REMOVE:
      syscall_remove_5(f);
      break;
    case SYS_OPEN:
      syscall_open_6(f);
      break;
    case SYS_FILESIZE:
      syscall_filesize_7(f);
      break;
    case SYS_READ:
      syscall_read_8(f);
      break;
    case SYS_WRITE:
      syscall_write_9(f);
      break;
    case SYS_SEEK:
      syscall_seek_10(f);
      break;
    case SYS_TELL:
      syscall_tell_11(f);
      break;
    case SYS_CLOSE:
      syscall_close_12(f);
      break;
    default:
      break;
  }
}