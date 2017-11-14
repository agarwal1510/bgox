#ifndef _MEM_H
#define _MEM_H

#include <sys/defs.h>
/*struct page {
	struct page *next;
	int used;
	int block_size;
};*/

//#define KERNEL_VADDR 0xffffffff80000000

void calculate_free_list(uint64_t num_pages, uint64_t physfree);
uint64_t *kmalloc(uint64_t size);
void free(uint64_t *ptr);
void memset(void *address, int value, int size);
#endif
