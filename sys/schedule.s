.text

.global context_switch

context_switch:
	push %rdi
	push %rdx
	mov %rsp, %rdi
	mov %rsi, %rsp
	pop %rdx
	mov %rdi, (%rdx)
	pop %rdi
	retq
