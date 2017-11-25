#ifndef _THREADS_H
#define _THREADS_H

#include <sys/defs.h>

#define STACK_SIZE 4096

struct vm_area_struct {
	struct mm_struct *vm_mm;
	uint64_t vm_start;
	uint64_t vm_end;
	uint64_t vm_file;
	uint64_t vm_pgoff;
	uint64_t vm_flags;
	struct vm_area_struct *vm_next;
};

struct mm_struct {
	uint32_t count;
	struct vm_area_struct *mmap;
};


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
struct vm_area_struct *vma_malloc(struct mm_struct *mm);

#endif
