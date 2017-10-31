#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ptops.h>

struct page *free_list;
struct page *fl_head;
struct page *fl_tail;


struct page *get_page_at_address(uint64_t *address) {
	uint64_t page_num = (uint64_t)address/(uint64_t)PAGE_SIZE; 
	return (struct page*)free_list + page_num*sizeof(struct page); 
}

int get_num_pages_allocated(uint64_t *ptr) {
	uint64_t page_num = (uint64_t)ptr/(uint64_t)PAGE_SIZE;
	struct page *corresponding_page = free_list + page_num*sizeof(struct page);
	return corresponding_page->pages_allocated;
}

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
	int pages;
	if (size % PAGE_SIZE > 0) {
		pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	} else {
		pages = size/PAGE_SIZE;
	}
	int avail_pages = (fl_tail - fl_head)/sizeof(struct page);
	uint64_t allocated = (fl_head - free_list)/sizeof(struct page);
	kprintf("Avail pages: %d allocated: %d\n", avail_pages, allocated);
	if (avail_pages < pages)
		return NULL;
	uint64_t *ret = (uint64_t *)(allocated*PAGE_SIZE);
	fl_head->pages_allocated = pages;
	while (pages != 0) {
		fl_head->used = 1;
		fl_head = fl_head->next;
		pages--;
	}
	//fl_head += sizeof(struct page)*pages;	
	return ret;
}

void free(uint64_t *ptr){
	int num_pages = get_num_pages_allocated(ptr);
	kprintf("Pages Allocated: %d", num_pages);
	struct page *address_page = get_page_at_address(ptr);
	memset(ptr, 0, num_pages*PAGE_SIZE);
	address_page->pages_allocated = 0;
	while(num_pages != 0) {
		address_page->used = 0;
		address_page = address_page->next;
		num_pages--;
	}
}

void memset (void *address, int value, int size) {
		unsigned char *p = address;
		for (int i = 0; i< size; i++)
			*p++ = (unsigned char)value;       
}

