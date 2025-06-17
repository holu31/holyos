.altmacro
.text
.extern interrupt_handler

interrupt_common_stub:
	push %r15
	push %r14
	push %r13
	push %r12
	push %rbp
	push %rbx
	push %r11
	push %r10
	push %r9
	push %r8
	push %rax
	push %rcx
	push %rdx
	push %rsi
	push %rdi	
	
	mov %rsp, %rdi
	call interrupt_handler
	
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rax
	pop %r8
	pop %r9
	pop %r10
	pop %r11
	pop %rbx
	pop %rbp
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	
	add $16, %rsp
	iretq

.macro int_noerr num
	int_stub_\num:
		pushq $0
		pushq $\num
		jmp interrupt_common_stub
.endm

.macro int_err num
	int_stub_\num:
		pushq $\num
		jmp interrupt_common_stub
.endm

int_noerr 0		# Divide Error
int_noerr 1		# Debug
int_noerr 2		# NMI
int_noerr 3		# Breakpoint
int_noerr 4		# Overflow
int_noerr 5		# Bound Range Exceeded
int_noerr 6		# Invalid Opcode
int_noerr 7		# Device Not Available
int_err   8		# Double Fault
int_noerr 9		# Coprocessor Segment Overrun
int_err   10		# Invalid TSS
int_err   11		# Segment Not Present
int_err   12		# Stack-Segment Fault
int_err   13		# General Protection Fault
int_err   14		# Page Fault
int_noerr 15		# Reserved
int_noerr 16		# x87 Floating-Point Exception
int_noerr 17		# Alignment Check
int_noerr 18		# Machine Check
int_noerr 19		# SIMD Floating-Point Exception
int_noerr 20		# Virtualization Exception
int_err	  21		# Control Protection Exception
int_noerr 22		# Reserved
int_noerr 23		# Reserved
int_noerr 24		# Reserved
int_noerr 25		# Reserved
int_noerr 26		# Reserved
int_noerr 27		# Reserved
int_noerr 28		# Hypervisor Injection Exception
int_err   29		# VMM Communication Exception
int_err   30		# Security Exception
int_noerr 31		# Reserved

.set i, 32
.rept 256
	int_noerr %i
	.set i, i + 1
.endr

.macro int_label num
	.quad int_stub_\num
.endm

.global interrupt_stub_table
interrupt_stub_table:
	.set i, 0
	.rept 256
		int_label %i
		.set i, i + 1
	.endr

