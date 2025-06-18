.altmacro
.text
.extern interrupt_handler

interrupt_common_stub:
	.align 16
	subq $120, %rsp
	movq %r15, 0(%rsp)
    	movq %r14, 8(%rsp)
	movq %r13, 16(%rsp)
	movq %r12, 24(%rsp)
	movq %rbp, 32(%rsp)
	movq %rbx, 40(%rsp)
	movq %r11, 48(%rsp)
	movq %r10, 56(%rsp)
	movq %r9, 64(%rsp)
	movq %r8, 72(%rsp)
	movq %rax, 80(%rsp)
	movq %rcx, 88(%rsp)
	movq %rdx, 96(%rsp)
	movq %rsi, 104(%rsp)
	movq %rdi, 112(%rsp)
	cld
	movq %rsp, %rdi
	call interrupt_handler

	movq 0(%rsp), %r15
	movq 8(%rsp), %r14
	movq 16(%rsp), %r13
	movq 24(%rsp), %r12
	movq 32(%rsp), %rbp
	movq 40(%rsp), %rbx
	movq 48(%rsp), %r11
	movq 56(%rsp), %r10
	movq 64(%rsp), %r9
	movq 72(%rsp), %r8
	movq 80(%rsp), %rax
	movq 88(%rsp), %rcx
	movq 96(%rsp), %rdx
	movq 104(%rsp), %rsi
	movq 112(%rsp), %rdi
	addq $136, %rsp
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

