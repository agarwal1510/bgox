#include <sys/threads.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>


void switch_to(task_struct *next, task_struct *me);

ready_task *ready_queue = NULL;
ready_task *head = NULL;
ready_task *previous = NULL;



void add_to_task_list(task_struct *process) {

	if (ready_queue == NULL){
		ready_queue = (ready_task *)kmalloc(sizeof(ready_task));
		ready_queue->process = process;
		ready_queue->next = NULL;
		head = ready_queue;
		return;
	}

	while(ready_queue->next != NULL){
		ready_queue = ready_queue->next;
	}

	ready_task *new_task = (ready_task *)kmalloc(sizeof(ready_task));
	new_task->process = process;
	new_task->next = NULL;
	ready_queue->next  = new_task;
	ready_queue = head;
}

void schedule() {
	if (head->next != NULL) {
		previous = head;
		head = head->next;
	} else {
		previous = head;
		head = ready_queue;			
	}
	//kprintf("next: %p me: %p", head->thread, previous->thread);
	//		if (make == 1)
	switch_to(head->process, previous->process);

}

void switch_to(task_struct *next, task_struct *me) {
	kprintf("Nextrsp: %p\n", next->rsp);

/*	__asm__ __volatile__( "pushq %rax");
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

*/
	__asm__ volatile ("movq %%rsp, %0" : "=m"(me->rsp));
	
	     __asm__ volatile ( "movq %0, %%cr3;"
	              :: "r" (next->cr3));
	
	__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));

/*
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
*/	
//	__asm__ __volatile__("iretq");
}

/*
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

	u_process1 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr = &thread1;

	for(int i = 0; i < STACK_SIZE; i ++){
		u_process1->kstack[i] = 0;
	}

	u_process1->kstack[STACK_SIZE - 1] = 0x23;
	u_process1->kstack[STACK_SIZE - 2] = (uint64_t)(&(u_process1->kstack[STACK_SIZE - 1]));
	u_process1->kstack[STACK_SIZE - 3] = 0x200286; //rflags  
	u_process1->kstack[STACK_SIZE - 4] = 0x1b; //cs
	u_process1->kstack[STACK_SIZE - 5] = (uint64_t)funcptr;  //rip entry point scroll down to see the value 
	
	for(int i = 0; i < 15; i++){
		u_process1->kstack[STACK_SIZE - 6 - i] = i;	
	}
//	u_process1->rip = (uint64_t)funcptr;
	u_process1->rsp = (uint64_t)(&(u_process1->kstack[STACK_SIZE - 2]));



	u_process2 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		u_process2->kstack[i] = 0;
	}

	u_process2->kstack[STACK_SIZE - 1] = 0x23;
	u_process2->kstack[STACK_SIZE - 2] = (uint64_t)(&(u_process2->kstack[STACK_SIZE - 1]));
	u_process2->kstack[STACK_SIZE - 3] = 0x200286; //rflags  
	u_process2->kstack[STACK_SIZE - 4] = 0x1b; //cs
	u_process2->kstack[STACK_SIZE - 5] = (uint64_t)funcptr2;  //rip entry point scroll down to see the value 
	
	for(int i = 0; i < 15; i++){
		u_process2->kstack[STACK_SIZE - 6 - i] = i;	
	}
	u_process2->rip = (uint64_t)funcptr2;
	u_process2->rsp = (uint64_t)(&(u_process2->kstack[STACK_SIZE - 20]));

	add_to_ktask_list(u_process1);
	add_to_ktask_list(u_process2);
		schedule();
	//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
//	thread1();
}*/
