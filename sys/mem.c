#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

struct page *free_list;
struct page *fl_head;
struct page *fl_tail;
uint64_t *pml4, *pdpt, *pdt;
uint64_t *pte[512];

void memset(void *address, int value, int size);

struct page *get_page_at_address(uint64_t *address) {
	uint64_t page_num = (uint64_t)address/(uint64_t)PAGE_SIZE; 
	return (struct page*)free_list + page_num*sizeof(struct page); 
}

int get_pages_allocated(uint64_t *ptr) {
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
	int num_pages = get_pages_allocated(ptr);
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

void memset (void *address, int value, int size) {                                                                                      unsigned char *p = address;                                                                                             for (int i = 0; i< size; i++)                                                                                            *p++ = (unsigned char)value;       
}

void init_page_table(uint64_t num_pages) {	
	uint64_t num_pte = num_pages/PT_SIZE;
	kprintf("Num pte %d %d", num_pages, num_pte);
	pml4 = (uint64_t *)kmalloc(PAGE_SIZE);
	pdpt = (uint64_t *)kmalloc(PAGE_SIZE);
	pdt = (uint64_t *)kmalloc(PAGE_SIZE);
	*pml4 = (uint64_t)pdpt;
	*pdpt = (uint64_t)pdt;
	for (int i = 0; i < num_pte; i++) {
		pte[i] = (uint64_t *)kmalloc(PAGE_SIZE);
		pdt[i] = (uint64_t)(pte[i]);	
	}
	// PT_SIZE 512
	// PAGE_SIZE 4096
	uint64_t inc = 0;
	for (int i = 0; i < num_pte; i++) {
		for (int j = 0; j < PT_SIZE; j++) {
			pte[i][j] = KERNEL_VADDR + inc*PAGE_SIZE;
			inc++;
		}
	}
	kprintf("%p %p %p %p", pte[0][0], pte[1][0], pte[0][1], pte[2][1]);
	kprintf("Address: %p %p %p %p %p %p %p\n", pml4, pdpt, *pdt, *(pdt+1), *pml4, *(pte), *(pte+1));
	
}

//uint64_t translate_vaddr_physaddr(uint64_t) {
//
//}
