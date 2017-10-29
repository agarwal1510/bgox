#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

struct page *free_list;


void calculate_free_list(uint64_t num_pages, uint64_t physfree) {
	free_list = (struct page*)physfree;
	struct page* temp_head = (struct page*)physfree;
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

	//Init pages occupied by freelist in freelist->used = 1;
	int used_flspace = num_pages*sizeof(struct page)/PAGE_SIZE;
	while(used_flspace != 0){
		free_ptr->used = 1;
		free_ptr = free_ptr->next;
		used_flspace--;
	}
	free_list = temp_head;
	int free = 0;
	int used = 0;
	while(free_list->used != -1){
		if (free_list->used == 1)
			used ++;
		else if (free_list->used == 0)
			free++;
		free_list = free_list->next;
	}
	kprintf("Free list stats: used: %d Free: %d\n", used, free);
}
