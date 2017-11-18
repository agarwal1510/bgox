#include <sys/threads.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>

//extern void context_switch();
//extern void pushAllReg();
//extern void popAllReg();

void switch_to(struct tcb *next, struct tcb *me);

struct tcb *process1, *process2;
struct ktask_entry *tasks = NULL;
struct ktask_entry *head = NULL;
struct ktask_entry *previous = NULL;

void add_to_ktask_list(struct tcb *process) {

	if (tasks == NULL){
		tasks = (struct ktask_entry *)kmalloc(sizeof(struct ktask_entry));
		tasks->thread = process;
		tasks->next = NULL;
		head = tasks;
		return;
	}

	while(tasks->next != NULL){
		tasks = tasks->next;
	}

	struct ktask_entry *new_task = (struct ktask_entry *)kmalloc(sizeof(struct ktask_entry));
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

void switch_to(struct tcb *next, struct tcb *me) {
	//kprintf("Nextrsp: %p\n", next->rsp);
	//kprintf("Mersp: %p\n", me->rsp);

	__asm__ __volatile__( "pushq %rax");
	__asm__ __volatile__( "pushq %rbx");
	__asm__ __volatile__( "pushq %rcx");
	__asm__ __volatile__( "pushq %rdx");
	__asm__ __volatile__( "pushq %rdi");
	__asm__ __volatile__( "pushq %rsi");
	__asm__ __volatile__( "pushq %rbp");
	__asm__ __volatile__( "pushq %r8");
	__asm__ __volatile__( "pushq %r9");
	__asm__ __volatile__( "pushq %r10");
	__asm__ __volatile__( "pushq %r11");
	__asm__ __volatile__( "pushq %r12");
	__asm__ __volatile__( "pushq %r13");
	__asm__ __volatile__( "pushq %r14");
	__asm__ __volatile__( "pushq %r15");


	__asm__ volatile ("movq %%rsp, %0" : "=m"(me->rsp));

	__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));


	__asm__ __volatile__( "popq %r15");
	__asm__ __volatile__( "popq %r14");
	__asm__ __volatile__( "popq %r13");
	__asm__ __volatile__( "popq %r12");
	__asm__ __volatile__( "popq %r11");
	__asm__ __volatile__( "popq %r10");
	__asm__ __volatile__( "popq %r9");
	__asm__ __volatile__( "popq %r8");
	__asm__ __volatile__( "popq %rbp");
	__asm__ __volatile__( "popq %rsi");
	__asm__ __volatile__( "popq %rdi");
	__asm__ __volatile__( "popq %rdx");
	__asm__ __volatile__( "popq %rcx");
	__asm__ __volatile__( "popq %rbx");
	__asm__ __volatile__( "popq %rax");
	__asm__ __volatile__("retq");
}

void thread1() {
	while(1) {
		kprintf("This is Thread 1\n");
		schedule();
		kprintf("Schedule 1 return");
	}
}

void thread2() {
	while (1) {
		kprintf("This is Thread 2\n");
		schedule();
		kprintf("Schedule 2 return");
	}
}



void switch_thread(){	

	process1 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr = &thread1;

	for(int i = 0; i < STACK_SIZE; i ++){
		process1->kstack[i] = 0;
	}

	process1->kstack[STACK_SIZE - 1] = (uint64_t)funcptr;
	process1->rip = (uint64_t)funcptr;
	process1->rsp = (uint64_t)(&(process1->kstack[STACK_SIZE - 1]));

	process2 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		process2->kstack[i] = 0;
	}
	kprintf("Stack bound: %p\n", (uint64_t)&process2->kstack[STACK_SIZE-1]);
	process2->kstack[STACK_SIZE - 1] = (uint64_t)funcptr2;
	process2->rip = (uint64_t)funcptr2;
	process2->rsp = (uint64_t)(&(process2->kstack[STACK_SIZE - 16]));

	add_to_ktask_list(process1);
	add_to_ktask_list(process2);
	//	schedule();
	//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
	thread1();
}

void switch_user_thread(){	
/*
	process1 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr = &thread1;

	for(int i = 0; i < STACK_SIZE; i ++){
		process1->kstack[i] = 0;
	}

	process1->kstack[STACK_SIZE - 1] = (uint64_t)funcptr;
	process1->rip = (uint64_t)funcptr;
	process1->rsp = (uint64_t)(&(process1->kstack[STACK_SIZE - 1]));

	process2 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		process2->kstack[i] = 0;
	}
	kprintf("Stack bound: %p\n", (uint64_t)&process2->kstack[STACK_SIZE-1]);
	process2->kstack[STACK_SIZE - 1] = (uint64_t)funcptr2;
	process2->rip = (uint64_t)funcptr2;
	process2->rsp = (uint64_t)(&(process2->kstack[STACK_SIZE - 16]));

	add_to_task_list(process1);
	add_to_task_list(process2);
	//	schedule();
	//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
	thread1();
*/}
