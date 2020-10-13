#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <list.h>
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "vm/page.h"

struct frame
{
  void* frame_addr;
  struct thread* holder;
  struct list_elem elem;
  struct page* pt;
};

struct list frame_table;

struct frame* alloc_frame(enum palloc_flags frame_flag, struct page* _page);

void remove_frame(struct frame* goal);

#endif