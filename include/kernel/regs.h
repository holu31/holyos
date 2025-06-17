#ifndef _REGS_H
#define _REGS_H

#include <stdint.h>

struct trap_frame {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t bp;
	uint64_t bx;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t ax;
	uint64_t cx;
	uint64_t dx;
	uint64_t si;
	uint64_t di;
	uint64_t orig_ax;
	uint64_t err;
	uint64_t ip;
	uint64_t cs;
	uint64_t flags;
	uint64_t sp;
	uint64_t ss;
} __attribute__((packed));

#endif // _REGS_H
