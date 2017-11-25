#ifndef _THREADS_H
#define _THREADS_H

#include <sys/defs.h>

#define STACK_SIZE 4096

struct tcb {
	uint64_t rsp;
	uint64_t kstack[STACK_SIZE];
	uint64_t pid;
	uint64_t cr3;
	uint64_t rip;
	enum {RUNNING, SLEEPING, ZOMBIE} state;
	int exit_status;
};


struct ktask_entry {
	struct ktask_entry *next;
	struct tcb *thread;
	int data;
};

void add_to_ktask_list(struct tcb *process);
void switch_thread();
void switch_user_thread();
void schedule();
#endif
