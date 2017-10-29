#include <sys/defs.h>
#include <sys/memgr.h>

uint64_t *fl_bmap;
uint64_t *mem_initref;

void memset(void *address, int value, int size) {
		unsigned char *p = address;
		for (int i = 0; i< size; i++)
				*p++ = (unsigned char)value;
}

void mem_init(uint64_t avail_pages, uint64_t physfree){
	
	fl_bmap = (uint64_t *)(physfree);

	uint64_t fl_numbytes = (avail_pages / 8) + 1;

	mem_initref = fl_bmap + fl_numbytes;

	memset(fl_bmap, 0, fl_numbytes); //Initialise entire memory to 0 (FreeList + Memreference)
	mark_page_use(fl_bmap, fl_numbytes/PAGE_SIZE+1);
}


uint64_t *kmalloc(uint32_t size){
	
	

}

// mark_use
// get_free
// mark_free
void mark_use(uint64_t *fl_bmap, int numpages){
	

}
uint64_t *get_free_page(){
	
	uint64_t *iterator = fl_bmap;
	
	uint64_t firstfree = *iterator & (0x01);
	int count = 0;
	while(firstfree != 0 && iterator != meminitref){
		
		if (*iterator == 0){
			iterator++;
		}

		firstfree = *iterator & (0x1);
		*iterator >>= 1;
		count++;
	}
	
	return (uint64_t *)(count*PAGE_SIZE + fl_bmap);
}
