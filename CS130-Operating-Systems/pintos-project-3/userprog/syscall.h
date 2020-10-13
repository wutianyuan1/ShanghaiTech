#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/interrupt.h"

void syscall_init (void);

int  filesys_try_open(char * filename);
void filesys_close_all(int tid);
void syscall_shutdown_0(struct intr_frame *f);
void syscall_exit_1(struct intr_frame *f);
void syscall_exec_2(struct intr_frame *f);
void syscall_wait_3(struct intr_frame *f);
void syscall_create_4(struct intr_frame *f);
void syscall_remove_5(struct intr_frame *f);
void syscall_open_6(struct intr_frame *f);
void syscall_filesize_7(struct intr_frame* f);
void syscall_read_8(struct intr_frame* f);
void syscall_write_9(struct intr_frame *f);
void syscall_seek_10(struct intr_frame *f);
void syscall_tell_11(struct intr_frame *f);
void syscall_close_12(struct intr_frame *f);

#endif /* userprog/syscall.h */
