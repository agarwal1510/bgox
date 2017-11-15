#ifndef _THREADS_H
#define _THREADS_H

#include <sys/defs.h>

#define STACK_SIZE 512

struct pcb {
	uint64_t kstack[STACK_SIZE];
	uint64_t pid;
	uint64_t rsp;
	uint64_t cr3;
	uint64_t rip;
	enum {RUNNING, SLEEPING, ZOMBIE} state;
	int exit_status;
};

struct task_entry {
	struct task_entry *next;
	struct pcb *thread;
	int data;
};

void switch_thread();
#endif
