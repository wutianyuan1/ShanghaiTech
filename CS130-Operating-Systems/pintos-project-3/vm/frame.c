#include <list.h>
#include <hash.h>
#include <stdio.h>
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/malloc.h"

static bool initalized = false;

static void frame_table_init(void);

static void frame_table_init()
{
  if (!initalized)
  {
    list_init(&frame_table);
    initalized = true;
  }
}

struct frame* alloc_frame(enum palloc_flags frame_flag, struct page* _page)
{
  frame_table_init();
  void* frame_content = palloc_get_page(frame_flag);
  if (!frame_content)
  {
    PANIC("CNMLGB");
    return NULL;
  }
  struct frame* new_frame = malloc(sizeof(struct frame));
  new_frame->holder = thread_current();
  new_frame->frame_addr = frame_content;
  new_frame->pt = _page;
  list_push_back(&frame_table, &new_frame->elem);
  return new_frame;
}

void remove_frame(struct frame* goal)
{
  palloc_free_page(goal->frame_addr);
  list_remove(&goal->elem);
  free(goal);
}


