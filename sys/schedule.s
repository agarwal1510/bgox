.text

.global pushAllReg
.global popAllReg

.global context_switch

pushAllReg:
pushq %rax
pushq %rbx
pushq %rcx
pushq %rdx
pushq %rdi
pushq %r8
pushq %r9
pushq %r10
pushq %r11

popAllReg:
popq %r11
popq %r10
popq %r9
popq %r8
popq %rdi
popq %rdx
popq %rcx
popq %rbx
popq %rax

#rdi -> me, rsi ->next
context_switch:
	call pushAllReg
	mov %rsp, %rdi
	mov %rsi, %rsp
	call popAllReg
	retq


