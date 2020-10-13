#include <list.h>
#include <stdio.h>
#include "vm/page.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "threads/synch.h"
#include "vm/frame.h"

static bool is_init = false;
static bool my_install_page (void *upage, void *kpage, bool writable);

static struct lock pagelock;


static bool
my_install_page (void *upage, void *kpage, bool writable)
{
  struct thread *t = thread_current ();

  /* Verify that there's not already a page at that virtual
     address, then map our page there. */
  return (pagedir_get_page (t->pagedir, upage) == NULL
          && pagedir_set_page (t->pagedir, upage, kpage, writable));
}

struct page*
create_page(uint8_t* _vaddr, bool _writeable, int _read_bytes, int _zero_bytes, struct file* _file, enum palloc_flags _flag, off_t _offset)
{
  if (!is_init)
  {
    list_init(&page_table);
    lock_init(&pagelock);
    is_init = true;
  }
  struct page* new_page = malloc(sizeof(struct page));
  if (!new_page) 
  {
    return NULL;
  }
  new_page->fr = NULL;
  new_page->loaded = false;
  new_page->owner = thread_current();
  new_page->flag = _flag;
  new_page->vaddr = _vaddr;
  new_page->file = _file;
  new_page->writeable = _writeable;
  new_page->read_bytes = _read_bytes;
  new_page->zero_bytes = _zero_bytes;
  new_page->offset = _offset;
  list_push_back(&page_table, &new_page->elem);
  return new_page;
}

struct page*
find_page_by_addr(uint32_t* find_vaddr)
{
  for (struct list_elem* iter = list_begin(&page_table);
       iter != list_end(&page_table); iter = list_next(iter))
  {
    struct page* current_page = list_entry(iter, struct page, elem);
    if (current_page->vaddr == find_vaddr)
      return current_page;
  }
  return NULL;
}

void alloc_page(struct page* p)
{
  struct frame* f = alloc_frame(p->flag, p);
  if (!f)
  {
    printf("BAD PAGE ALLOC\n");
    return;
  }
  p->fr = f;
  p->loaded = true;
}

void grow_stack(void* fault_addr)
{
  void* round_addr = pg_round_down((const void*)fault_addr);
  //printf("grow stack: handled, fault addr: %p, round to: %p\n", fault_addr, round_addr);

  struct page* fault_page = find_page_by_addr(round_addr);
  if (!fault_page)
  {
    //printf("grow stack: not found fault page\n");
    fault_page = create_page(round_addr, 1, 0, 0, NULL, PAL_USER | PAL_ZERO, 0);
    alloc_page(fault_page);
    bool res = my_install_page(fault_page->vaddr, fault_page->fr->frame_addr, fault_page->writeable);
    if (!res) 
      goto done;
    //printf("handle end\n");
  }
  else
  {
    // printf("grow stack: found it\n");
    alloc_page(fault_page);
    //printf("frame addr: %p\n", fault_page->fr->frame_addr);
    fault_page->file = file_reopen(fault_page->file);
    //printf("reopen file: %p, need read %d bytes\n", fault_page->file, fault_page->read_bytes);
    int offs = file_read_at(fault_page->file, fault_page->fr->frame_addr, fault_page->read_bytes, fault_page->offset);
    //printf("read file %d\n", offs);
    memset (fault_page->fr->frame_addr + fault_page->read_bytes, 0, fault_page->zero_bytes);
    bool res = my_install_page(fault_page->vaddr, fault_page->fr->frame_addr, fault_page->writeable);
    if (!res) 
      goto done;
    //printf("handle end\n");
  }
  done:
    return;
}
