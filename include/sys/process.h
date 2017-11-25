#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/defs.h>
#include <sys/threads.h>

#define MAX_PROCESSES 1024
static int PID = 0;

typedef struct vm_area_struct {
	uint64_t vma_start;
	uint64_t vma_end;
	uint64_t vma_file;
	uint64_t vma_offset;
	long vma_flags;
	uint64_t vma_mmsz;
//	mm_struct *mm; //Pointer to its mm struct
	struct vm_area_struct *next_vma;
} vm_area_struct;

typedef struct mm_struct {
	struct vm_area_struct *mmap;
	int count;
} mm_struct;

typedef struct task_struct {
	uint64_t rsp;
	uint64_t kstack[STACK_SIZE];
	uint64_t *ustack;
	uint64_t pid;
	uint64_t cr3;
	char tname[100];
	mm_struct *mm;
	uint64_t pml4;
	int exit_status;
} task_struct;

task_struct *ready_queue[1024];
uint64_t num_processes = 0;
#endif
