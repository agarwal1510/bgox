#ifndef _PTMGR_H
#define _PTMGR_H

#include <sys/defs.h>

#define PAGE_SIZE  4096
#define PAGES_PER_PTE 512
#define PAGES_PER_PD 512
#define PAGES_PER_PML4 512
#define PAGES_PER_PDP 512
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x1ff)
#define PAGE_DIR_INDEX(x) (((x) >> 21) & 0x1ff)
#define PAGE_PDP_INDEX(x) (((x) >> 30) & 0x1ff)
#define PAGE_PML4_INDEX(x) (((x) >> 39) & 0x1ff)
#define KERNEL_VADDR 0xffffffff80000000
#define VADDR(PADD) (KERNEL_VADDR + (uint64_t)PADD)
#define PADDR(VADD) ((uint64_t)VADD - KERNEL_VADDR)
#define IS_PRESENT_PAGE(addr) ((uint64_t)addr & 1)
#define RW_KERNEL_FLAGS 7UL
#define ENTRIES_PER_PTE 512

struct page {
	struct page *next;
	int used;
	int block_size;
};

typedef uint64_t pde_entry;
typedef uint64_t pte_entry;
typedef uint64_t pdp_entry;
typedef uint64_t pml4_entry;

typedef struct ptable {
	pte_entry pt [PAGES_PER_PTE];
}ptable;

typedef struct pdtable {
	pde_entry pd [PAGES_PER_PD];
}pdtable;

typedef struct pdptable {
	pdp_entry pdp [PAGES_PER_PDP];
}pdptable;

typedef struct pml4table {
	pml4_entry pml4 [PAGES_PER_PML4];
}pml4table;

enum PAGE_PTE_FLAGS {
	PTE_PRESENT			=	1,		//0000000000000000000000000000001
	PTE_WRITABLE		=	2,		//0000000000000000000000000000010
	PTE_USER			=	4,		//0000000000000000000000000000100
	PTE_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	PTE_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	PTE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	PTE_DIRTY			=	0x40,		//0000000000000000000000001000000
	PTE_PAT				=	0x80,		//0000000000000000000000010000000
	PTE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	PTE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
	PTE_FRAME			=	0x0000FFFFFFFFF000,	//1111111111111111111000000000000
};

enum PAGE_PDE_FLAGS {
	PDE_PRESENT			=	1,		//0000000000000000000000000000001
	PDE_WRITABLE		=	2,		//0000000000000000000000000000010
	PDE_USER			=	4,		//0000000000000000000000000000100
	PDE_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	PDE_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	PDE_PAT				=	0x80,		//0000000000000000000000010000000
	PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	PDE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
	PDE_FRAME			=	0x0000FFFFFFFFF000,	//1111111111111111111000000000000
};

enum PAGE_PDP_FLAGS {
	PDP_PRESENT			=	1,		//0000000000000000000000000000001
	PDP_WRITABLE		=	2,		//0000000000000000000000000000010
	PDP_USER			=	4,		//0000000000000000000000000000100
	PDP_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	PDP_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	PDP_ACCESSED		=	0x20,		//0000000000000000000000000100000
	PDP_DIRTY			=	0x40,		//0000000000000000000000001000000
	PDP_PAT				=	0x80,		//0000000000000000000000010000000
	PDP_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	PDP_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
	PDP_FRAME			=	0x0000FFFFFFFFF000,	//1111111111111111111000000000000
};

enum PAGE_PML4_FLAGS {
	PML4_PRESENT			=	1,		//0000000000000000000000000000001
	PML4_WRITABLE		=	2,		//0000000000000000000000000000010
	PML4_USER			=	4,		//0000000000000000000000000000100
	PML4_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	PML4_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	PML4_ACCESSED		=	0x20,		//0000000000000000000000000100000
	PML4_DIRTY			=	0x40,		//0000000000000000000000001000000
	PML4_PAT				=	0x80,		//0000000000000000000000010000000
	PML4_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	PML4_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
	PML4_FRAME			=	0x0000FFFFFFFFF000,	//1111111111111111111000000000000
};
//void init_page_table(uint64_t num_pages);
//void vmem_init(void* physbase);
void init_paging(uint64_t kernmem, uint64_t physbase, uint64_t num_pages);
#endif
