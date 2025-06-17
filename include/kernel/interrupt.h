#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <stdint.h>
#include <kernel/regs.h>

typedef void (*irq_handler_t)(struct trap_frame *r);

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_attr;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

void idt_init();
void irq_request(uint8_t irq, irq_handler_t handler);

#endif // _INTERRUPT_H
