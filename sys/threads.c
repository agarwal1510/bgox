#include <sys/threads.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>

extern void context_switch();
extern void pushAllReg();
extern void popAllReg();

void switch_to(struct pcb *next, struct pcb *me);

struct pcb *process1, *process2;
struct task_entry *tasks = NULL;
struct task_entry *head = NULL;
struct task_entry *previous = NULL;

int i = 0;
void add_to_task_list(struct pcb *process) {

	if (tasks == NULL){
		tasks = (struct task_entry *)kmalloc(sizeof(struct task_entry));
		tasks->thread = process;
		tasks->next = NULL;
		head = tasks;
		return;
	}

	while(tasks->next != NULL){
		tasks = tasks->next;
	}

	struct task_entry *new_task = (struct task_entry *)kmalloc(sizeof(struct task_entry));
	new_task->thread = process;
	new_task->next = NULL;
	tasks->next  = new_task;
	tasks = head;
}

void schedule() {
	if (head->next != NULL) {
		previous = head;
		head = head->next;
	} else {
		previous = head;
		head = tasks;			
	}
	//kprintf("next: %p me: %p", head->thread, previous->thread);
	//		if (make == 1)
	switch_to(head->thread, previous->thread);

}

void pushAll() {

	//		__asm__ __volatile__( "pushq %rax");
	//		__asm__ __volatile__( "pushq %rbx");
	//		__asm__ __volatile__( "pushq %rcx");
	//		__asm__ __volatile__( "pushq %rbp");
	__asm__ __volatile__( "pushq %rdi");
	__asm__ __volatile__( "pushq %rsi");
	__asm__ __volatile__( "pushq %rdx");
	//		__asm__ __volatile__( "pushq %r8");
	//		__asm__ __volatile__( "pushq %r9");
	//		__asm__ __volatile__( "pushq %r10");
	//		__asm__ __volatile__( "pushq %r11");
	//		__asm__ __volatile__( "pushq %r12");
	//		__asm__ __volatile__( "pushq %r13");
	//		__asm__ __volatile__( "pushq %r14");
	//		__asm__ __volatile__( "pushq %r15");
}

void popAll() {

	//		__asm__ __volatile__( "popq %r15");
	//		__asm__ __volatile__( "popq %r14");
	//		__asm__ __volatile__( "popq %r13");
	//		__asm__ __volatile__( "popq %r12");
	//		__asm__ __volatile__( "popq %r11");
	//		__asm__ __volatile__( "popq %r10");
	//		__asm__ __volatile__( "popq %r9");
	//		__asm__ __volatile__( "popq %r8");
	__asm__ __volatile__( "popq %rdx");
	__asm__ __volatile__( "popq %rsi");
	__asm__ __volatile__( "popq %rdi");
	//		__asm__ __volatile__( "popq %rbp");
	//		__asm__ __volatile__( "popq %rcx");
	//		__asm__ __volatile__( "popq %rbx");
	//		__asm__ __volatile__( "popq %rax");
}
void switch_to(struct pcb *next, struct pcb *me) {
	kprintf("Nextrsp: %p\n", next->rsp);
	kprintf("Mersp: %p\n", me->rsp);

	//int i = 0;
	i++;
	//		pushAll();
	__asm__ __volatile__( "pushq %rax");
	__asm__ __volatile__( "pushq %rbx");
	__asm__ __volatile__( "pushq %rcx");
	__asm__ __volatile__( "pushq %rdx");
	__asm__ __volatile__( "pushq %rdi");
	__asm__ __volatile__( "pushq %rsi");
//	__asm__ __volatile__( "pushq %rbp");
//	__asm__ __volatile__( "pushq %r8");
//	__asm__ __volatile__( "pushq %r9");
//	__asm__ __volatile__( "pushq %r10");
//	__asm__ __volatile__( "pushq %r11");
//	__asm__ __volatile__( "pushq %r12");
//	__asm__ __volatile__( "pushq %r13");
//	__asm__ __volatile__( "pushq %r14");
//	__asm__ __volatile__( "pushq %r15");
	//if (i == 2) {
	//__asm__ __volatile__ ("movq %0, %%rsp;"::"m"(me->rsp):"memory");
	//}
	//__asm__ volatile ("movq %%rsp, %0" : "=m"(me->rsp));
	//		__asm__ __volatile__( "pushq %rbp");
	//__asm__ __volatile__ ("movq %rsp, %rbp");
	//		__asm__ __volatile__( "pushq %rax");

	//		__asm__ __volatile__( "pushq %rcx");	
	//		__asm__ __volatile__( "pushq %rdx");

	//		__asm__ __volatile__( "pushq %rdi");
	//		__asm__ __volatile__( "pushq %rsi");
	//__asm__ __volatile__("pushq %rdx");
	//	__asm__ __volatile__( "pushq %rbx");
	//	__asm__ __volatile__("pushq %r12");
	//	__asm__ __volatile__("pushq %r13");
	//	__asm__ __volatile__("pushq %r14");
	//	__asm__ __volatile__("pushq %r15");
	//		if (i == 2) {
	if (i >= 2) {
	__asm__ volatile ("movq %%rsp, %0" : "=m"(me->rsp));
	kprintf("Me-rsp: %p", me->rsp);
	}
	//		next->rsp = me->rsp;
	//		kprintf("Mersp: %p\n", me->rsp);
	//		while(1);
	//		} else if (i == 3) {
	//			while(1);
	//		}

	uint64_t p;
	__asm__ __volatile__ ("movq %0, %%rsp;"::"m"(next->rsp):"memory");
	__asm__ volatile ("movq %%rsp, %0;" : "=m" (p));
	kprintf("\nRsp: %p\n", p);
//	__asm__ __volatile__( "popq %r15");
//	__asm__ __volatile__( "popq %r14");
//	__asm__ __volatile__( "popq %r13");
//	__asm__ __volatile__( "popq %r12");
//	__asm__ __volatile__( "popq %r11");
//	__asm__ __volatile__( "popq %r10");
//	__asm__ __volatile__( "popq %r9");
//	__asm__ __volatile__( "popq %r8");
//	__asm__ __volatile__( "popq %rbp");
	__asm__ __volatile__( "popq %rsi");
	__asm__ __volatile__( "popq %rdi");
	__asm__ __volatile__( "popq %rdx");
	__asm__ __volatile__( "popq %rcx");
	__asm__ __volatile__( "popq %rbx");
	__asm__ __volatile__( "popq %rax");
	//__asm__ __volatile__ ("movq %rbp, %rsp");
	//__asm__ __volatile__ ("popq %rbp");
	//		popAll();
	//__asm__ __volatile__ ("movq %%rsp, %0;":"=m"(me->rsp));
	__asm__ volatile ("movq %%rsp, %0;" : "=m" (p));
	kprintf("\nRSP: %p\n", p);
	__asm__ __volatile__("retq");
}

void thread1() {
	//while(1) {
	kprintf("This is Thread 1\n");
	schedule();
	kprintf("Schedule 1 return");
	while(1);
	//}
}

void thread2() {
	//while (1) {
	kprintf("This is Thread 2\n");
	//schedule();
	kprintf("Schedule 2 return");
	while(1);
	//}
}


void switch_thread(){	

	process1 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr = &thread1;
	//	23
	// 511

	for(int i = 0; i < STACK_SIZE; i ++){
		process1->kstack[i] = 0;
	}

	process1->kstack[STACK_SIZE - 1] = (uint64_t)funcptr;
	process1->kstack[STACK_SIZE - 2] = (uint64_t)funcptr;
	//	process1->kstack[STACK_SIZE - 2] = (uint64_t)(process1);
	//	process1->kstack[STACK_SIZE - 3] = 0x246;
	//	process1->kstack[STACK_SIZE - 4] = 0x1b;
	//	process1->kstack[STACK_SIZE - 5] = 0x1b;
	//	process1->kstack[STACK_SIZE - 6] = ;

	//	process1->kstack[STACK_SIZE - 2] = 0x08;
	//	process1->kstack[STACK_SIZE - 3] = 0x200202UL;
	//	process1->kstack[STACK_SIZE - 4] = 0x08;
	//	process1->kstack[STACK_SIZE - 5] = 0x08;
	process1->rip = (uint64_t)funcptr;
	//	process1->kstack[STACK_SIZE - 21] = (uint64_t)context_switch;
	process1->rsp = (uint64_t)(&(process1->kstack[STACK_SIZE - 2]));


	process2 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		process2->kstack[i] = 0;
	}

	process2->kstack[STACK_SIZE - 1] = (uint64_t)funcptr;
	process2->kstack[STACK_SIZE - 2] = (uint64_t)funcptr;
	//	process2->kstack[STACK_SIZE - 2] = (uint64_t)(process2);
	//	process2->kstack[STACK_SIZE - 3] = 0x200202UL;
	//	process2->kstack[STACK_SIZE - 4] = 0x08;
	//	process2->kstack[STACK_SIZE - 5] = 0x08;
	process2->rip = (uint64_t)funcptr2;
	//	process2->kstack[STACK_SIZE - 21] = (uint64_t)context_switch;
	process2->rsp = (uint64_t)(&(process2->kstack[STACK_SIZE - 2]));

	add_to_task_list(process1);
	add_to_task_list(process2);
	//	schedule();
//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
	thread1();
}
