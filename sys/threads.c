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

void schedule(int make) {
		if (head->next != NULL) {
				previous = head;
				head = head->next;
		} else {
				previous = head;
				head = tasks;			
		}
		kprintf("next: %p me: %p", head->thread, previous->thread);
//		if (make == 1)
		switch_to(head->thread, previous->thread);
}

void switch_to(struct pcb *next, struct pcb *me) {
		kprintf("Nextrsp: %p\n", next->rsp);
		kprintf("Mersp: %p\n", me->rsp);

//		__asm__ __volatile__(
//						"movq %%rsp, %0;"
//						:"=m"(me->rsp)
//						:   
//						:"memory"
//						);  

		kprintf("Mersp: %p\n", me->rsp);
//		kprintf("Me: %p\n", me->kstack);
//		__asm__ __volatile__( "call pushAllReg");
//		__asm__ __volatile__( "push %rax");
//		__asm__ __volatile__ (
//						"movq %%rsp, %%rdi;"
//						:   
//						:
//						:"memory"
//						);
		__asm__ __volatile__( "pushq %rax");
		__asm__ __volatile__( "pushq %rbx");
		__asm__ __volatile__( "pushq %rcx");
		__asm__ __volatile__( "pushq %rdx");
		__asm__ __volatile__( "pushq %rbp");
		__asm__ __volatile__( "pushq %rsi");
		__asm__ __volatile__( "pushq %rdi");
//		__asm__ __volatile__( "movq %rsp, %rdi");
//		__asm__ __volatile__( "pushq %rdi");
		__asm__ __volatile__ (
						"movq %%rsp, %0;"
						:   
						:"m"(me->rsp)
						:
						);
		__asm__ __volatile__ (
						"movq %0, %%rsp;"
						:   
						:"m"(next->rsp)
						:"memory"
						);
//		__asm__ __volatile__( "popq %rdi");
		__asm__ __volatile__( "popq %rdi");
		__asm__ __volatile__( "popq %rsi");
		__asm__ __volatile__( "popq %rbp");
		__asm__ __volatile__( "popq %rdx");
		__asm__ __volatile__( "popq %rcx");
		__asm__ __volatile__( "popq %rbx");
		__asm__ __volatile__( "popq %rax");

		__asm__ __volatile__("retq");
		kprintf("%s","here here" );
		//		context_switch(me, next);
}

void thread1() {
//		while(1) {
				//		uint64_t p = 0;
				//		__asm__ __volatile__(
				//						"movq %%rsp, %0;"
				//						:"=m"(p)
				//						:   
				//						:"memory"
				//						);  
				//		kprintf("Next-> %p", p);
				kprintf("This is Thread 1\n");
				schedule(1);

				kprintf("Schedule 1 return");
//		}
			while(1){}
}

void thread2() {
//		while (1) {
				kprintf("This is Thread 2\n");
				schedule(0);
//		}
			while(1){}
		//	return;
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
//	thread1();
}
