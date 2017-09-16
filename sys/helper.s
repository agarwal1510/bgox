.text

.global _x86_64_asm_lidt
.global isr0

.extern irq_handler

timer_init: 
	movw	$119, %dx
 
	movb	$0x36, %al
	out	%al, $0x43
 
 
	movw	%ax, %dx
	outb	%al, $0x40
	xchg	%ah, %al
	outb	%al, $0x40
	ret

_x86_64_asm_lidt:
	lidt (%rdi)
	sti
	retq

isr0:
	cli
	call irq_handler
	sti
	iretq
