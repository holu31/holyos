.section .text

.global gdt_init
.type gdt_init, @function
gdt_init:
	lgdt gdt_ptr(%rip)

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	pushq $0x08
	leaq .reload_cs(%rip), %rax # Текущий RIP-relative адрес
	pushq %rax
	lretq
    
.reload_cs:
	retq

.section .rodata
.align 8

gdt:
	.quad 0x0000000000000000
	.quad 0x00af9a000000ffff
	.quad 0x00af92000000ffff
	.quad 0x00affa000000ffff
	.quad 0x00aff2000000ffff

gdt_ptr:
	.word . - gdt - 1
	.quad gdt
