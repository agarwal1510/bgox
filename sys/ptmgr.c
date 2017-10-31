#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ptops.h>
#include <sys/ptmgr.h>

uint64_t *pml4, *pdpt, *pdt;
uint64_t *pte[512];

void init_page_table(uint64_t num_pages) {	
		uint64_t num_pte = num_pages/PAGE_SIZE;
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
	// PAGE_S 512
	// PAGE_SIZE 4096
	uint64_t inc = 0;
	for (int i = 0; i < num_pte; i++) {
		for (int j = 0; j < PAGE_SIZE; j++) {
			pte[i][j] = KERNEL_VADDR + inc*PAGE_SIZE;
			inc++;
		}
	}
	kprintf("%p %p %p %p", pte[0][0], pte[1][0], pte[0][1], pte[2][1]);
	kprintf("Address: %p %p %p %p %p %p %p\n", pml4, pdpt, *pdt, *(pdt+1), *pml4, *(pte), *(pte+1));
	
}

// PTE ROUTINE START

bool allocate_page_pt(pte_entry *e){
	void *p = kmalloc(1);
	// Allocate a single page;

	if (p == NULL)
		return false;

	pte_entry_set_frame(e, (uint64_t)p);
	pte_entry_add_attr(e, PTE_PRESENT);
	
	return true;
}

void free_page_pt(pte_entry *e){

	void *p = (void *)pte_entry_get_pfn(*e);
	if (p)
		free(p);
	
	pte_entry_del_attr(e, PTE_PRESENT);
}

inline pte_entry * lookup_pte_entry(ptable *p, uint64_t virt_addr){

	if (p)
		return &p->pt[PAGE_TABLE_INDEX(virt_addr)];
	return 0;
}

// PTE ROUTINES END

// PDT ROUTINES START

inline pdp_entry* lookup_pdp_entry(pdptable *p, uint64_t virt_addr){

	if (p)
		return &p->pdp[PAGE_PDP_INDEX(virt_addr)];
	return 0;
}

inline pml4_entry* lookup_pml4_entry(pml4table *p, uint64_t virt_addr){

	if (p)
		return &p->pd[PAGE_PML4_INDEX(virt_addr)];
	return 0;
}
