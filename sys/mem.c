#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ptops.h>

struct page *free_list;

void memset(void *address, int value, int size);

struct page *get_page_at_address(uint64_t *address) {
	uint64_t page_num = (uint64_t)address/(uint64_t)PAGE_SIZE; 
	return (struct page*)free_list + page_num*sizeof(struct page); 
}

int get_num_pages_allocated(uint64_t *ptr) {
	uint64_t page_num = (uint64_t)ptr/(uint64_t)PAGE_SIZE;
	struct page *corresponding_page = free_list + page_num*sizeof(struct page);
	return corresponding_page->block_size;
}

void calculate_free_list(uint64_t num_pages, uint64_t physfree) {
	free_list = (struct page*)physfree;
	uint64_t used_pages = physfree/0x1000;
	uint64_t free_pages = num_pages - used_pages;
	int used_ctr = 0, free_ctr = 0;
	while (used_pages != 0) {
		free_list->used = 1;
		free_list->block_size = used_pages;
		free_list->next = free_list+sizeof(struct page);
		free_list = free_list->next;
		used_pages--;
		used_ctr++;
	}
	struct page *free_ptr = free_list;

	while (free_pages != 0) {
		free_list->used = 0;
		free_list->block_size = free_pages;
		free_list->next = free_list+sizeof(struct page);
		free_list = free_list->next;
		free_pages--;
		free_ctr++;
	}
	free_list->used = -1;
	//Init pages occupied by freelist in freelist->used = 1;
	int used_flspace = num_pages*sizeof(struct page)/PAGE_SIZE;
	kprintf("loop ctr: %d %d %d", used_ctr, free_ctr, used_flspace);
	while(used_flspace != 0){
		free_ptr->used = 1;
		free_ptr->block_size = used_flspace;
		free_ptr = free_ptr->next;
		used_flspace--;
	}
	kprintf("\nLast free list addr: %p", (uint64_t)free_ptr);
	free_list = (struct page*)physfree;
}

uint64_t *kmalloc(uint64_t size){
	int pages;
	uint64_t count = 0;
	if (size % PAGE_SIZE > 0) {
		pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	} else {
		pages = size/PAGE_SIZE;
	}
	struct page *temp = free_list;
	kprintf("Temp addr: %p", (uint64_t)temp);
	while(temp != NULL) {
		if (temp->used == 0 && temp->block_size >= pages) {
			temp->used = 1;
			temp->block_size = pages;
			uint64_t *ret = (uint64_t *)(count*PAGE_SIZE);
			temp = temp->next;
			while (--pages != 0) {
				temp->used = 1;
				temp->block_size = pages;
				temp = temp->next;
			}
			return ret;
		}
		temp = temp->next;
		count++;
	}
	return NULL;
}

void free(uint64_t *ptr){
	int num_pages = get_num_pages_allocated(ptr);
	kprintf("Pages Allocated: %d", num_pages);
	struct page *address_page = get_page_at_address(ptr);
	memset(ptr, 0, num_pages*PAGE_SIZE);
	while(num_pages != 0) {
		address_page->used = 0;
		address_page->block_size = num_pages;
		address_page = address_page->next;
		num_pages--;
	}	
}

void memset (void *address, int value, int size) {
		unsigned char *p = address;
		for (int i = 0; i< size; i++)
			*p++ = (unsigned char)value;       
}

