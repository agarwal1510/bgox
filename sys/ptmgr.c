#include <sys/mem.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ptops.h>
#include <sys/paging.h>
#include <sys/mem.h>

uint64_t *pml4, *pdpt, *pdt;
uint64_t *pte[512];

pml4table* current_pml4 = 0;
uint64_t curr_pml4br = 0;

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

void ptmgr_paging_enable (bool b) {
	
	uint64_t cr0;
	__asm__ volatile ("mov %%cr0, %0;" : "=b" (cr0):);
	if (b) {
		cr0 |= 0x80000000;
		__asm__ volatile ("mov %0, %%cr0;" :: "b" (cr0));
	} else {
		cr0 &= 0x7FFFFFFF;
		__asm__ volatile ("mov %0, %%cr0;" :: "b" (cr0));
	}
}


void ptmgr_load_PML4BR (uint64_t addr) {

	__asm__ volatile ( "mov %0, %%cr3;"
			 :: "r" (addr));
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

	//void *p = (void *)pte_entry_get_pfn(*e);
	//if (p)
	//	free(p);
	
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
		return &p->pml4[PAGE_PML4_INDEX(virt_addr)];
	return 0;
}
inline bool ptmgr_switch_pml4 (pml4table* pml4) {

		if (!pml4)
				return false;

		current_pml4 = pml4;
		ptmgr_load_PML4BR(curr_pml4br);
		return true;
}

pml4table* get_pml4(){

	return current_pml4;

}

void map_virt_phys(void * phys, void * virt){
	
	pml4table *pml4_table = get_pml4();

	pml4_entry *e = &pml4_table->pml4[PAGE_PML4_INDEX((uint64_t)virt)];

	if ((*e & PML4_PRESENT) != PML4_PRESENT){
			pdptable *pdp = (pdptable *)kmalloc(1);
			if (pdp == NULL)
					return;

			memset(pdp, 0, 0x1000);

			pml4_entry* pml4entry = &pml4_table->pml4[PAGE_PML4_INDEX((uint64_t)virt)];
			pml4_entry_add_attr(pml4entry, PML4_PRESENT);
			pml4_entry_add_attr(pml4entry, PML4_WRITABLE);
			pml4_entry_set_frame(pml4entry, (uint64_t)pdp);

			pdtable *pd = (pdtable *)kmalloc(1);
			memset(pd, 0, 0x1000);
			
			pdp_entry *pdpentry = &pdp->pdp[PAGE_PDP_INDEX((uint64_t)virt)];
			pdp_entry_add_attr(pdpentry, PDP_PRESENT);
			pdp_entry_add_attr(pdpentry, PDP_WRITABLE);
			pdp_entry_set_frame(pdpentry, (uint64_t)pd);

			ptable *pt = (ptable *)kmalloc(1);
			memset(pd, 0, 0x1000);
			
			pde_entry *pdentry = &pd->pd[PAGE_DIR_INDEX((uint64_t)virt)];
			pde_entry_add_attr(pdentry, PDE_PRESENT);
			pde_entry_add_attr(pdentry, PDE_WRITABLE);
			pde_entry_set_frame(pdentry, (uint64_t)pt);
			
			pte_entry *ptentry = &pt->pt[PAGE_TABLE_INDEX((uint64_t)virt)];
			pte_entry_add_attr(ptentry, PTE_PRESENT);
			pte_entry_add_attr(ptentry, PTE_WRITABLE);
			pte_entry_set_frame(ptentry, (uint64_t)phys);
		
			
		// Extend for PDP, PD, PT and populate page table entry

	}
}

void vmem_init(void *physbase){


		ptable* table = (ptable*) kmalloc (1);
		if (!table)
				return;

		//! allocates 3gb page table
//		ptable* table2 = (ptable*) kmalloc (1);
//		if (!table2)
//				return;

		//! clear page table
//		vimmngr_ptable_clear (table);
		memset(table, 0, 0x1000);

//		for (int i=0, frame=0x0, virt=0x00000000; i<512; i++, frame+=4096, virt+=4096) {

				//! create a new page
//				pte_entry page=0;
//				pte_entry_add_attr(&page, PTE_PRESENT);
//				pte_entry_set_frame (&page, frame);

				//! ...and add it to the page table
//				table->pt[PAGE_TABLE_INDEX(virt)] = page;
//		}
		pml4table* pml4 = (pml4table*) kmalloc(1);
		if (!pml4)
			return;
		memset (pml4, 0, sizeof (pml4));
		
		pdptable* pdp = (pdptable*) kmalloc(1);
		if (!pdp)
			return;
		memset (pdp, 0, sizeof (pdp));
		
		pdtable* pd = (pdtable*) kmalloc(1);
		if (!pd)
			return;
		memset (pd, 0, sizeof (pd));
		

		for (int i=0, frame=(uint64_t)physbase, virt=(uint64_t)(KERNEL_VADDR+physbase); i<512; i++, frame+=4096, virt+=4096) {

				//! create a new page
				pte_entry page=0;
				pte_entry_add_attr(&page, PTE_PRESENT);
				pte_entry_add_attr(&page, PTE_WRITABLE);
				pte_entry_set_frame (&page, frame);

				//! ...and add it to the page table
				table->pt[PAGE_TABLE_INDEX (virt) ] = page;

				pde_entry pde = 0;
				pde_entry_add_attr(&pde, PDE_PRESENT);
				pde_entry_add_attr(&pde, PDE_WRITABLE);
				pde_entry_set_frame(&pde, (uint64_t)table);
				pd->pd[PAGE_DIR_INDEX((virt))] = pde;
				
				pdp_entry pdpe = 0;
				pdp_entry_add_attr(&pdpe, PDP_PRESENT);
				pdp_entry_add_attr(&pdpe, PDP_WRITABLE);
				pdp_entry_set_frame(&pdpe, (uint64_t)pd);
				pdp->pdp[PAGE_PDP_INDEX((virt))] = pdpe;
				
				pml4_entry pml4e = 0;
				pml4_entry_add_attr(&pml4e, PDE_PRESENT);
				pml4_entry_add_attr(&pml4e, PDE_WRITABLE);
				pml4_entry_set_frame(&pml4e, (uint64_t)pdp);
				pml4->pml4[PAGE_PML4_INDEX((uint64_t)(virt))] = pml4e;
				
//				kprintf("%p %p %p %p\n", pml4->pml4[PAGE_PML4_INDEX((uint64_t)(virt))], pdpe, pde, page);
				//kprintf("V: %p T %p", virt, frame);
		}
		uint64_t virt = (uint64_t)(KERNEL_VADDR+physbase);
		kprintf("\nVIRTUAL: %p\n", (uint64_t)virt);
//		uint64_t *base = 
//		kprintf("\ntest: %p\n",((uint64_t)0x20000000 << (uint64_t)3));
//		kprintf("\nSHIFT: %p %d\n",(uint64_t)physbase, (uint64_t)(KERNEL_VADDR+physbase) >> 12);
//		kprintf("\nTRANSLATE: %p\n", (uint64_t)pte_entry_get_pfn(table->pt[PAGE_TABLE_INDEX(virt)]));
		
		
//		pdptable* pdp2 = (pdptable*) kmalloc(1);
//		if (!pdp2)
//			return;
//		memset (pdp2, 0, sizeof (pdp2));
		
//		pdtable* pd2 = (pdtable*) kmalloc(1);
//		if (!pd2)
//			return;
//		memset (pd2, 0, sizeof (pd2));

//		pml4_entry* pml4e = &pml4->pml4 [PAGE_PML4_INDEX((uint64_t)(KERNEL_VADDR+physbase))];
//		pml4_entry_add_attr (pml4e, PML4_PRESENT);
///		pml4_entry_add_attr (pml4e, PML4_WRITABLE);
//		pml4_entry_set_frame (pml4e, (uint64_t)pdp);
//		
//		pml4_entry* pml4e2 = &pml4->pml4 [PAGE_PML4_INDEX((uint64_t)(0x0))];
//		pml4_entry_add_attr (pml4e2, PML4_PRESENT);
//		pml4_entry_add_attr (pml4e2, PML4_WRITABLE);
//		pml4_entry_set_frame (pml4e2, (uint64_t)pdp2);

//		pdp_entry* pdpe = &pdp->pdp[PAGE_PDP_INDEX((uint64_t)(KERNEL_VADDR+physbase))];
//		pdp_entry_add_attr (pdpe, PDP_PRESENT);
//		pdp_entry_add_attr (pdpe, PDP_WRITABLE);
//		pdp_entry_set_frame (pdpe, (uint64_t)pd);
		
//		pdp_entry* pdpe2 = &pdp->pdp[PAGE_PDP_INDEX((uint64_t)(0x0))];
//		pdp_entry_add_attr (pdpe2, PDP_PRESENT);
//		pdp_entry_add_attr (pdpe2, PDP_WRITABLE);
//		pdp_entry_set_frame (pdpe2, (uint64_t)pd2);
		
//		pde_entry* pde = &pd->pd[PAGE_DIR_INDEX((uint64_t)(KERNEL_VADDR+physbase))];
//		pde_entry_add_attr (pde, PDE_PRESENT);
//		pde_entry_add_attr (pde, PDE_WRITABLE);
//		pde_entry_set_frame (pde, (uint64_t)table);
		
//		pde_entry* pde2 = &pd->pd[PAGE_DIR_INDEX((uint64_t)(0x0))];
//		pde_entry_add_attr (pde2, PDE_PRESENT);
//		pde_entry_add_attr (pde2, PDE_WRITABLE);
//		pde_entry_set_frame (pde2, (uint64_t)table2);
	//! clear directory table and set it as current
		curr_pml4br = (uint64_t)&pml4->pml4;

		ptmgr_switch_pml4 (pml4);
		ptmgr_paging_enable(true);
}
