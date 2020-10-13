#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <list.h>
#include "vm/frame.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

struct page
{
  struct list_elem elem;
  struct frame* fr;
  struct thread* owner;
  enum palloc_flags flag;
  uint32_t* vaddr;
  bool writeable;
  int read_bytes;
  int zero_bytes;
  struct file* file;
  off_t offset;
  bool loaded;
};

/* return page struct by given addr */
struct page* find_page_by_addr(uint32_t* find_vaddr);

/* do memory allocation for a given page */
void alloc_page(struct page* p);

/* just create a page, no memory allocation */
struct page* create_page(uint8_t* _vaddr, bool _writeable, int _read_bytes, int _zero_bytes, struct file* _file, enum palloc_flags _flag, off_t offset);

void grow_stack(void* fault_addr);

struct list page_table;

#endif