#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel/regs.h>

#define SYSCALL_INT	0x80
#define SYSCALL_NUM	10

typedef int syscall_t(struct trap_frame*);

void syscall_init();

#endif // _SYSCALL_H
