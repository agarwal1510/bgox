#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

struct page *free_list;
struct page *fl_head;
struct page *fl_tail;

void calculate_free_list(uint64_t num_pages, uint64_t physfree) {
	free_list = (struct page*)physfree;
	uint64_t used_pages = physfree/0x1000;
	uint64_t free_pages = num_pages - used_pages;
	int loop_ctr = 0;
	while (used_pages != 0) {
		free_list->used = 1;
		free_list->next = free_list+sizeof(struct page);
		free_list = free_list->next;
		used_pages--;
		loop_ctr++;
	}
	struct page *free_ptr = free_list;
	
	while (free_pages != 0) {
		free_list->used = 0;
		free_list->next = free_list+sizeof(struct page);
		free_list = free_list->next;
		free_pages--;
		loop_ctr++;
	}
	free_list->used = -1;
	fl_tail = free_list;
	//Init pages occupied by freelist in freelist->used = 1;
	int used_flspace = num_pages*sizeof(struct page)/PAGE_SIZE;
	while(used_flspace != 0){
		free_ptr->used = 1;
		free_ptr = free_ptr->next;
		used_flspace--;
	}
	fl_head = free_ptr;
	free_list = (struct page*)physfree;
}

uint64_t *kmalloc(uint64_t size){
	int pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	
	int avail_pages = (fl_tail - fl_head)/sizeof(struct page);
	uint64_t allocated = (fl_head - free_list)/sizeof(struct page);
	kprintf("Avail pages: %d allocated: %d\n", avail_pages, allocated);
	if (avail_pages < pages)
		return NULL;
	uint64_t *ret = (uint64_t *)(allocated*PAGE_SIZE);
	fl_head += sizeof(struct page)*pages;	
	return ret;
}
void free(uint64_t *ptr){

}
