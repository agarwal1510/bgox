#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/idt.h>

static void *syscalls[1] = {
	&kprintf
};

//DEFN_SYSCALL1(kprintf, 0, const char*);
//DEFN_SYSCALL1(write_hex, 1, const char*);

uint32_t num_syscalls = 1;


void syscall_handler(registers_t *regs){
	while(1);
	if (regs->eax >= num_syscalls)
		return;

	void *location = syscalls[regs->eax];

	int ret;

	__asm__ volatile (" \
			push %1; \
			push %2; \
			push %3; \
			push %4; \
			push %5; \
			call *%6; \
			pop %%rbx; \
			pop %%rbx; \
			pop %%rbx; \
			pop %%rbx; \
			pop %%rbx; \
			": "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));
	regs->eax = ret;



}

void syscall_init(){
	setup_gate(0x80, (uint64_t)&syscall_handler);
}
