.text

.global _x86_64_asm_lidt
.global isr0
.global isr1
.global timer_init
.extern irq_timer_handler
.extern irq_kb_handler

timer_init:
	movw	$11931, %dx
	movb	$0x36, %al
	outb	%al, $0x43
	movw	%dx, %ax
	outb	%al, $0x40
	xchg	%al, %ah
	outb	%al, $0x40
	ret

_x86_64_asm_lidt:
	lidt (%rdi)
	sti
	retq

isr0:
	cli
	call irq_timer_handler
	sti
	iretq

isr1:
	cli
	call irq_kb_handler
	sti
	iretq
