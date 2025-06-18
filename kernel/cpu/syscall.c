#include <kernel/syscall.h>
#include <kernel/printf.h>
#include <kernel/interrupt.h>

syscall_t* calls[SYSCALL_NUM];

int syscall_write(struct trap_frame* r) {
	if (r->di) {
		printf("write from syscall: %s", (char*)r->bx);
		return 0;
	}
	return -1;
}

void syscall_handler(struct trap_frame* r) {
	if (r->ax > SYSCALL_NUM) return;
	r->ax = calls[r->ax](r);

	asm volatile("movq %0, %%rax" :: "r"((r->ax)));
}

void syscall_init() {
	irq_request(SYSCALL_INT, &syscall_handler);

	calls[0] = &syscall_write;
}
