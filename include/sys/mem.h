#ifndef _MEM_H
#define _MEM_H

#include <sys/defs.h>
#include <sys/process.h>
/*struct page {
	struct page *next;
	int used;
	int block_size;
};*/

//#define KERNEL_VADDR 0xffffffff80000000

void calculate_free_list(uint64_t num_pages, uint64_t physfree);
uint64_t *kmalloc(uint64_t size);
uint64_t *kmalloc_init(uint64_t size);
uint64_t *kmalloc_stack(uint64_t size, uint64_t *pml4e);
void free(uint64_t *ptr);
void memset(void *address, int value, int size);
struct vm_area_struct *vma_malloc(struct mm_struct *mm);
void region_alloc(task_struct *pcb, uint64_t va, uint64_t size);
void dec_ref_count(uint64_t addr);
void inc_ref_count(uint64_t addr);
struct page *get_page_at_address(uint64_t addr);
int get_page_ref_count(uint64_t addr);
#endif
