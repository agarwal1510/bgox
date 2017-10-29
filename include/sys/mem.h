#ifndef _MEM_H
#define _MEM_H

#include <sys/defs.h>
struct page {
	struct page *next;
	struct page *previous;
	int used;
};

#define PAGE_SIZE 4096;
void calculate_free_list(uint64_t num_pages, uint64_t physfree);

#endif
