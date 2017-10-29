#ifndef _MEMGR_H
#define _MEMGR_H

#define PAGE_SIZE 4096

void mem_init(uint64_t avail_pages, uint64_t physfree);
void memset(void *address, int value, int size);


#endif
