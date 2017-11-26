#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/threads.h>

#define MAX_PROCESSES 1024
//static int PID = 0;

typedef struct task_struct {
	uint64_t *rsp;
	uint64_t kstack[STACK_SIZE];
	uint64_t *ustack;
	uint64_t pid;
	uint64_t *cr3;
	char tname[100];
	mm_struct *mm;
	uint64_t pml4;
	int exit_status;
	uint64_t entry;
} task_struct;

typedef struct ready_task {
	task_struct *process;
	struct ready_task *next;
	struct ready_task *prev;

}ready_task;
//task_struct *ready_queue;
//uint64_t num_processes = 0;
void schedule();
void add_to_task_list(task_struct *process);
#endif
