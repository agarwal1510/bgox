#ifndef _MEM_H
#define _MEM_H

#include <sys/defs.h>
struct page {
	struct page *next;
	struct page *previous;
	int used;
	int pages_allocated;
};

#define PAGE_SIZE  4096
#define KERNEL_VADDR 0xffffffff80000000
#define PT_SIZE 512
void calculate_free_list(uint64_t num_pages, uint64_t physfree);
uint64_t *kmalloc(uint64_t size);
void free(uint64_t *ptr);
void memset(void *address, int value, int size);
void init_page_table(uint64_t num_pages);
#endif
