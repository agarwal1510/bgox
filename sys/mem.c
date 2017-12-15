#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ptops.h>
#include <sys/paging.h>
#include <sys/utils.h>
#include <sys/process.h>

struct page *free_list;

void memset(void *address, int value, int size);

struct page *get_page_at_address(uint64_t address) {
	uint64_t addr;
	if (address > KERNEL_VADDR) {	
		addr = get_starting_page((uint64_t)PADDR(address));
	} else {	
		addr = get_starting_page((uint64_t)address);
	}
	uint64_t page_num = addr/(uint64_t)PAGE_SIZE; 
	//kprintf("Corr num: %d %p %p", page_num, free_list, address);
	struct page *corr = (struct page*)free_list + page_num*sizeof(struct page); 
	//TODO FREELIST HERE
	//kprintf("Corr: %p %p", corr, free_list);
	return corr;
}

void dec_ref_count(uint64_t addr) {
	struct page *p = get_page_at_address(addr);
	p->ref_count = p->ref_count - 1;
}

void inc_ref_count(uint64_t addr) {
	struct page *p = get_page_at_address(addr);
	p->ref_count = p->ref_count - 1;
}

int get_page_ref_count(uint64_t addr) {
	struct page *p = get_page_at_address(addr);
	return p->ref_count;
}

int get_num_pages_allocated(uint64_t *ptr) {
	uint64_t page_num = (uint64_t)ptr/(uint64_t)PAGE_SIZE;
	struct page *corresponding_page = free_list + page_num*sizeof(struct page);
	return corresponding_page->block_size;
}

void calculate_free_list(uint64_t num_pages, uint64_t physfree) {
//	kprintf("NUm pages: %d %d %p", num_pages, sizeof(struct page), physfree);
	free_list = (struct page*)physfree;
	uint64_t used_pages = physfree/0x1000;
	uint64_t free_pages = num_pages - used_pages;
//	kprintf("Used-free %d %d", used_pages, free_pages);
	int used_ctr = 0, free_ctr = 0;
	while (used_pages != 0) {
		free_list->used = 1;
		free_list->block_size = used_pages;
		free_list->ref_count = 1;
		free_list->next = free_list+1; //sizeof(struct page);
		free_list = free_list->next;
		used_pages--;
		used_ctr++;
	}
	struct page *free_ptr = free_list;
//	kprintf("%p ", free_ptr);

	while (free_pages != 0) {
		free_list->used = 0;
		free_list->block_size = free_pages;
		free_list->ref_count = 0;
		free_list->next = free_list+1; //sizeof(struct page);
		free_list = free_list->next;
		free_pages--;
		free_ctr++;
	}
	free_list->used = -1;
	//Init pages occupied by freelist in freelist->used = 1;
	int used_flspace = num_pages*sizeof(struct page)/PAGE_SIZE;
//	kprintf("loop ctr: %d %d %d %p", used_ctr, free_ctr, used_flspace, free_list);
	while(used_flspace != 0){
		free_ptr->used = 1;
		free_ptr->block_size = used_flspace;
		free_list->ref_count = 1;
		free_ptr = free_ptr->next;
		used_flspace--;
	}
//	kprintf("\nLast free list addr: %p", (uint64_t)free_list);
	free_list = (struct page*)physfree;
}

uint64_t *kmalloc_init(uint64_t size){
	
	int pages;
	uint64_t count = 0;
	if (size % PAGE_SIZE > 0) {
		pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	} else {
		pages = size/PAGE_SIZE;
	}
//	kprintf("Pages: %d %d", pages, size);
	struct page *temp = free_list;
	//kprintf("Temp addr: %p", (uint64_t)temp);
	while(temp != NULL) {
		if (temp->used == 0 && temp->block_size >= pages) {
			temp->used = 1;
			temp->block_size = pages;
			temp->ref_count = 1;
			uint64_t *ret = (uint64_t *)((count)*PAGE_SIZE);
			memset(ret, 0, pages*PAGE_SIZE);
			temp = temp->next;
			while (--pages != 0) {
				temp->used = 1;
				temp->block_size = pages;
				temp->ref_count = 1;
				temp = temp->next;
			}
//			if (flag == 1){
//			
//				kprintf("\n%p", ret);
//				while(1);
//			}

//			uint64_t base = KERNEL_VADDR;
		//	kprintf("Addr: %p", ret);
			return (uint64_t *)VADDR(ret);
		}
		temp = temp->next;
		count++;
	}
	return NULL;
}

void free(uint64_t *ptr){
	struct page *address_page = get_page_at_address((uint64_t)ptr);
	int num_pages = address_page->block_size;
	//kprintf("Pages Allocated: %p %d", address_page, num_pages);	
	memset(ptr, 0, num_pages*PAGE_SIZE);
	while(num_pages != 0) {
		address_page->used = 0;
		address_page->block_size = num_pages;
		address_page->ref_count = 0;
		address_page = address_page->next;
		num_pages--;
	}	
}

void region_alloc(task_struct *pcb, uint64_t va, uint64_t size) {
	uint64_t start = get_starting_page(va);
	uint64_t end = get_ending_page(va+size);
//	kprintf("\nstart: %p end: %p\n", start, end);
	uint64_t v;
	for (v = start; v < end; v += PAGE_SIZE) {
		uint64_t *addr = kmalloc(1);
		init_map_virt_phys_addr((uint64_t)v, PADDR(addr), 1, (uint64_t *)(pcb->pml4), 1);
	}
//	walk_page_table(va);
}
struct vm_area_struct *vma_malloc(struct mm_struct *mm) {

        struct vm_area_struct *vma;
        if (mm->mmap != NULL) {
                vma = mm->mmap;
                while (vma->vm_next != NULL) {
                        vma = vma->vm_next;
                }
                vma->vm_next = vma + sizeof(struct vm_area_struct);
                mm->count += 1;
                return vma->vm_next;

        } else {
                vma = (struct vm_area_struct *)kmalloc(sizeof(vm_area_struct));
                mm->mmap = vma;
                mm->count += 1;
                return vma;
        }
}



///////////////// MEMORY IN VADDR SPACE /////////////

uint64_t *kmalloc(uint64_t size){

	int pages;
	uint64_t count = 0;
	if (size % PAGE_SIZE > 0) {
		pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	} else {
		pages = size/PAGE_SIZE;
	}
	//kprintf("Pages: %d", pages);
	struct page *temp = (struct page *)VADDR(free_list);
	//kprintf("Temp addr: %p", (uint64_t)temp);
	while(temp != NULL) {
		if (temp->used == 0 && temp->block_size >= pages) {
			temp->used = 1;
			temp->block_size = pages;
			temp->ref_count = 1;
			uint64_t *ret = (uint64_t *)((count)*PAGE_SIZE);
			memset((uint64_t*)VADDR(ret), 0, pages*PAGE_SIZE);
			temp = (struct page*)VADDR(temp->next);
			while (--pages != 0) {
				temp->used = 1;
				temp->block_size = pages;
				temp->ref_count = 1;
				temp = (struct page*)VADDR(temp->next);
			}

			return (uint64_t *)VADDR(ret);
		}
		temp = (struct page *)VADDR(temp->next);
		count++;
	}
	return NULL;
}

uint64_t *kmalloc_stack(uint64_t size, uint64_t * pml4e){

	static uint64_t bumpPtr=(uint64_t )(0xFFFFF0F080700000);     
	int pages;
	uint64_t count = 0;
	if (size % PAGE_SIZE > 0) {
		pages = (size/PAGE_SIZE)+1; //TODO Fix pages when asked size = 4096KB
	} else {
		pages = size/PAGE_SIZE;
	}
	//kprintf("Pages: %d", pages);
	struct page *temp = (struct page *)VADDR(free_list);
	//kprintf("Temp addr: %p", (uint64_t)temp);
	while(temp != NULL) {
		if (temp->used == 0 && temp->block_size >= pages) {
			temp->used = 1;
			temp->block_size = pages;
			temp->ref_count = 1;
			uint64_t *ret = (uint64_t *)((count)*PAGE_SIZE);
			memset((uint64_t*)VADDR(ret), 0, pages*PAGE_SIZE);
			temp = (struct page *)VADDR(temp->next);
			while (--pages != 0) {
				temp->used = 1;
				temp->block_size = pages;
				temp->ref_count = 1;
				temp = (struct page*)VADDR(temp->next);
			}
//			kprintf("before init %p %p", bumpPtr, ret);
			init_map_virt_phys_addr((uint64_t)((uint64_t)bumpPtr+(uint64_t)ret), (uint64_t)ret, 1, pml4e, 1);
		//	walk_page_table((uint64_t)0xfffff0f080b14006);
			return (uint64_t *)((uint64_t)bumpPtr+(uint64_t)ret);
		}
		temp = (struct page *)VADDR(temp->next);
		count++;
	}
	return NULL;
}
