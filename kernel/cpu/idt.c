#include <kernel/string.h>
#include <kernel/interrupt.h>
#include <kernel/pic.h>
#include <kernel/printf.h>
#include <stdbool.h>

static struct idt_entry idt[256];
static struct idt_ptr idtr;

static irq_handler_t handlers[256];
extern void* interrupt_stub_table[];

static inline void load_idt(uint64_t ptr) {
	asm volatile("lidt %0" :: "m"(idtr));
}

void idt_set_entry(uint8_t vector, void* handler, uint8_t flags) {
	uint64_t offset = (uint64_t)handler;
	idt[vector].offset_low = offset & 0xFFFF;
	idt[vector].selector = 0x08;
	idt[vector].ist = 0;
	idt[vector].type_attr = flags;
	idt[vector].offset_mid = (offset >> 16) & 0xFFFF;
	idt[vector].offset_high = (offset >> 32) & 0xFFFFFFFF;
	idt[vector].reserved = 0;
}

void exception_handler(struct trap_frame regs) {
	handlers[regs.orig_ax](&regs);
}

void interrupt_handler(struct trap_frame regs) {
	if (regs.orig_ax < 32) {
		exception_handler(regs);
	} else {
		if (handlers[regs.orig_ax])
			handlers[regs.orig_ax](&regs);
		
		pic_send_eoi(regs.orig_ax - 32);
	}
}

void irq_request(uint8_t irq, irq_handler_t handler) {
	handlers[irq] = handler;
	
	if (irq >= 32 && irq < 48) {
		pic_unmask_irq(irq - 32);
	}
}

void page_fault_handler(struct trap_frame* regs) {
	uint64_t fault_address;
	asm volatile("mov %%cr2, %0" : "=r" (fault_address));

	bool present = regs->orig_ax & 0x1;
	bool write = regs->orig_ax & 0x2;
	bool user = regs->orig_ax & 0x4;
	bool reserved = regs->orig_ax & 0x8;
	bool instruction = regs->orig_ax & 0x10;

	printf("Page Fault at 0x%x\n", fault_address);
	printf("Error code: 0x%x\n", regs->orig_ax);
	printf("Details: %s %s %s %s\n",
           present ? "present" : "not-present",
           write ? "write" : "read",
           user ? "user" : "supervisor",
           instruction ? "instruction" : "data");

	asm volatile("hlt");
}

void idt_init() {
	idtr.limit = sizeof(idt) - 1;
	idtr.base = (uint64_t)&idt;

	memset(idt, 0, sizeof(struct idt_entry));

	for (int i = 0; i < 256; i++) {
		if (i < 32) {
			idt_set_entry(i, interrupt_stub_table[i], 0x8E);	
		}
		else {
			idt_set_entry(i, interrupt_stub_table[i], 0x8F);
		}
	}

	idt_set_entry(14, page_fault_handler, 0x8E);

	load_idt((uint64_t)&idtr);
}
