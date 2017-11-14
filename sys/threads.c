#include <sys/threads.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>

extern void context_switch();
void switch_to(struct pcb *next, struct pcb *me);
struct pcb *processes;
struct task_list *tasks = NULL;
struct task_list *head = NULL;
struct task_list *previous = NULL;

void add_to_task_list(struct pcb *process) {
	if (tasks == NULL) {
		tasks = (struct task_list *) kmalloc(sizeof(struct task_list));
	}
	if (tasks->next == NULL) {
		tasks->next = (struct task_list *) kmalloc(sizeof(struct task_list));
	}
	tasks = tasks->next;
	tasks->thread = process;
	tasks->next  = NULL;
}

void schedule() {
	if (head == NULL) {
		head = tasks;
	} else {
		if (head->next != NULL) {
			previous = head;
			head = head->next;
		} else {
			previous = head;
			head = tasks;			
		}
	}
	switch_to(head->thread, previous->thread);
}

void switch_to(struct pcb *next, struct pcb *me) {
//	context_switch();
}

void thread1() {
	while(1) {
		kprintf("This is Thread 1\n");
		schedule();
	}
}

void thread2() {
	while (1) {
		kprintf("This is Thread 2\n");
		schedule();
	}
}


void switch_thread() {
	
	
	struct pcb *process1 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr = thread1;
	process1->kstack[STACK_SIZE - 1] = 0x10;
	process1->kstack[STACK_SIZE - 2] = 0x08;
	process1->kstack[STACK_SIZE - 3] = 0x200202UL;
	process1->kstack[STACK_SIZE - 4] = 0x08;
	process1->kstack[STACK_SIZE - 5] = 0x08;
	process1->rip = (uint64_t)funcptr;
	process1->kstack[STACK_SIZE - 21] = (uint64_t)context_switch;
	process1->rsp = (uint64_t)(process1->kstack[STACK_SIZE - 22]);
	
	struct pcb *process2 = (struct pcb *) kmalloc(sizeof(struct pcb));
	void *funcptr2 = thread2;
	process2->kstack[STACK_SIZE - 1] = 0x10;
	process2->kstack[STACK_SIZE - 2] = 0x08;
	process2->kstack[STACK_SIZE - 3] = 0x200202UL;
	process2->kstack[STACK_SIZE - 4] = 0x08;
	process2->kstack[STACK_SIZE - 5] = 0x08;
	process2->rip = (uint64_t)funcptr2;
	process2->kstack[STACK_SIZE - 21] = (uint64_t)context_switch;
	process2->rsp = (uint64_t)(process1->kstack[STACK_SIZE - 22]);
	
	schedule();
}
