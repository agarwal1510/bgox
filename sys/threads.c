#include <sys/threads.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>

extern void context_switch();
void switch_to(struct pcb *next, struct pcb *me);
struct pcb *processes;
struct task_list *task_list = NULL;
struct task_list *head = NULL;
struct task_list *previous = NULL;

void schedule() {
	if (head == NULL) {
		head = task_list;
	} else {
		if (head->next != NULL) {
			previous = head;
			head = head->next;
		} else {
			previous = head;
			head = task_list;			
		}
	}
	switch_to(head->thread, previous->thread);
}

void switch_to(struct pcb *next, struct pcb *me) {
	
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
	void (*funcptr)() = &thread1;
	process1->kstack[STACK_SIZE - 1] = 0x10;
	process1->kstack[STACK_SIZE - 2] = 0x08;
	process1->kstack[STACK_SIZE - 3] = 0x200202UL;
	process1->kstack[STACK_SIZE - 4] = 0x08;
	process1->kstack[STACK_SIZE - 5] = 0x08;
	process1->rip = (*funcptr)();
	process1->kstack[STACK_SIZE - 21] = (uint64_t)context_switch;
	process1->rsp = (uint64_t)&process1->kstack[STACK_SIZE - 22];

	//uint64_t *process2_page = kmalloc(1);

}
