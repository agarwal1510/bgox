.text

.global pushAllReg
.global popAllReg

.global context_switch

pushAllReg:
pushq %rax
pushq %rcx
pushq %rdx
pushq %rsi
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
popq %rsi
popq %rdx
popq %rcx
popq %rax


context_switch:
	call pushAllReg
	mov %rsp, %rdi
	mov %rsi, %rsp
	call popAllReg
	retq


