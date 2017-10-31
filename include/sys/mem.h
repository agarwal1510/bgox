#ifndef _MEM_H
#define _MEM_H

#include <sys/defs.h>
struct page {
	struct page *next;
	int used;
	int block_size;
};



#define PAGE_SIZE  4096
#define KERNEL_VADDR 0xffffffff80000000
#define PT_SIZE 512

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

void calculate_free_list(uint64_t num_pages, uint64_t physfree);
uint64_t *kmalloc(uint64_t size);
void free(uint64_t *ptr);
void memset(void *address, int value, int size);
void init_page_table(uint64_t num_pages);
#endif
