#include <sys/mem.h>
#include <sys/kprintf.h>
#include <sys/paging.h>

//#define RW_KERNEL_FLAGS 7UL
//#define VADDR(PADDR) (KERNEL_VADDR + (uint64_t)PADDR)
//#define PAGESIZE 4096
//#define ENTRIES_PER_PTE 512
//#define IS_PRESENT_PAGE(addr) ((uint64_t)addr & 1)

uint64_t topVAddr;

void set_highest_virtaddr(uint64_t addr) {
	topVAddr = addr;
	kprintf("Top Address: %p\n", topVAddr);
}

uint64_t get_highest_virtaddr() {
	return topVAddr;
}



static uint64_t* alloc_pte(uint64_t *pde_table, int pde_off, int access)
{
	uint64_t *pte_table = (uint64_t *)PADDR(kmalloc(1));
	if (access == 1)
		pde_table[pde_off] = (uint64_t)(pte_table) | RW_USER_FLAGS;   
	else
		pde_table[pde_off] = (uint64_t)(pte_table) | RW_KERNEL_FLAGS;   
	return (uint64_t*)VADDR(pte_table);
} 

static uint64_t* alloc_pde(uint64_t *pdpe_table, int pdpe_off, int access)
{
	uint64_t* pde_table = (uint64_t *)PADDR(kmalloc(1));
	if (access == 1)
		pdpe_table[pdpe_off] = (uint64_t)(pde_table) | RW_USER_FLAGS;   
	else
		pdpe_table[pdpe_off] = (uint64_t)(pde_table) | RW_KERNEL_FLAGS;   
	return (uint64_t*)VADDR(pde_table);
}

static uint64_t* alloc_pdpe(uint64_t *pml4_table, int pml4_off, int access)
{
	//	kprintf("PML4 off: %d %p", pml4_off, pml4_table);
	uint64_t* pdpe_table = (uint64_t *)PADDR(kmalloc(1));
	if (access == 1)
		pml4_table[pml4_off] = (uint64_t)(pdpe_table) | RW_USER_FLAGS;   
	else
		pml4_table[pml4_off] = (uint64_t)(pdpe_table) | RW_KERNEL_FLAGS;   
	return (uint64_t*)VADDR(pdpe_table);
}


void init_map_virt_phys_addr(uint64_t vaddr, uint64_t paddr, uint64_t no_of_pages, uint64_t *pml4_table, int access)
{
	uint64_t *pdpe_table = NULL, *pde_table = NULL, *pte_table = NULL;

	uint64_t i, j, k, pde_off, pdpe_off, pml4_off , pte_off; 
	uint64_t phys_addr;
	pte_off  = (vaddr >> 12) & 0x1FF;
	pde_off  = (vaddr >> 21) & 0x1FF;
	pdpe_off = (vaddr >> 30) & 0x1FF;
	pml4_off = (vaddr >> 39) & 0x1FF;

	//     kprintf(" $$ NEW MAPPING $$ OFF => %p %d %d %d %d ", vaddr, pml4_off, pdpe_off, pde_off, pte_off);

	phys_addr = (uint64_t) *(pml4_table + pml4_off);
	    kprintf("\nPML4: %p %p %p",phys_addr, ker_pml4_t, pml4_off);
	if (IS_PRESENT_PAGE(phys_addr)) {
		phys_addr = phys_addr >> 12 << 12; 
		pdpe_table =(uint64_t*)phys_addr; 
		phys_addr = (uint64_t) *(pdpe_table + pdpe_off);
		kprintf("\nInside pml4: %p %p %d", phys_addr, pdpe_table, pdpe_off);
		if (IS_PRESENT_PAGE(phys_addr)) {
			phys_addr = phys_addr >> 12 << 12;
			pde_table =(uint64_t*) phys_addr; 

			phys_addr  = (uint64_t) *(pde_table + pde_off);
			kprintf("\nInside pdp: %p %p %d", phys_addr, pde_table, pde_off);
			if (IS_PRESENT_PAGE(phys_addr)) {
				phys_addr = phys_addr >> 12 << 12;
				pte_table =(uint64_t*) phys_addr;
				//		uint64_t ptee = (uint64_t) *(pte_table + pte_off);
				//		kprintf("\nInside pde: %p %p %p", phys_addr, pte_table, ptee);
			} else {
				pte_table = alloc_pte(pde_table, pde_off, access);
			}
		} else {
			pde_table = alloc_pde(pdpe_table, pdpe_off, access);
			pte_table = alloc_pte(pde_table, pde_off, access);
		}
	} else {
		pdpe_table = alloc_pdpe(pml4_table, pml4_off, access);
		pde_table = alloc_pde(pdpe_table, pdpe_off, access);
		pte_table = alloc_pte(pde_table, pde_off, access);
	}

	phys_addr = paddr;  
	//	kprintf("Pte off: %d", pte_off);
	if (no_of_pages + pte_off <= ENTRIES_PER_PTE) {
		for (i = pte_off; i < (pte_off + no_of_pages); i++) {
			if (access == 1)
				pte_table[i] = phys_addr | RW_USER_FLAGS; 
			else
				pte_table[i] = phys_addr | RW_KERNEL_FLAGS; 
			//kprintf("Phys: %p", phys_addr);
			//break;
			phys_addr += PAGE_SIZE;
		}
		//      kprintf("\n SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", no_of_pages, pdpe_table , pde_table, pte_table);
	} else {
		int lno_of_pages = no_of_pages, no_of_pte_t;

		//      kprintf("\n SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
		for ( i = pte_off ; i < ENTRIES_PER_PTE; i++) {
			if (access == 1)
				pte_table[i] = phys_addr | RW_USER_FLAGS;
			else
				pte_table[i] = phys_addr | RW_KERNEL_FLAGS;

			phys_addr += PAGE_SIZE;
		}
		lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE - pte_off);
		no_of_pte_t = lno_of_pages/ENTRIES_PER_PTE;

		for (j = 1; j <= no_of_pte_t; j++) {   
			pte_table = alloc_pte(pde_table, pde_off+j, access);
			//           kprintf("\n SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
			for(k = 0; k < ENTRIES_PER_PTE; k++ ) {
				if (access == 1)
					pte_table[k] = phys_addr | RW_USER_FLAGS;
				else
					pte_table[k] = phys_addr | RW_KERNEL_FLAGS;

				phys_addr += PAGE_SIZE;
			}
		}
		lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE * pte_off);
		pte_table = alloc_pte(pde_table, pde_off+j, access);

		//       kprintf("\n SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
		for(k = 0; k < lno_of_pages; k++ ) {
			if (access == 1)
				pte_table[k] = phys_addr | RW_USER_FLAGS;
			else
				pte_table[k] = phys_addr | RW_KERNEL_FLAGS;

			phys_addr += PAGE_SIZE;
		}
	}
}

void LOAD_CR3(uint64_t addr) {

	__asm__ volatile ( "movq %0, %%cr3;"
			:: "r" (addr));
}

void init_paging(uint64_t kernmem, uint64_t physbase, uint64_t num_pages)
{
	// Allocate free memory for PML4 table 
	ker_cr3 = (uint64_t *)PADDR(kmalloc(1));

	ker_pml4_t = (uint64_t *) VADDR(ker_cr3);
	//kprintf("\tKernel PML4t:%p", ker_pml4_t);

	//ker_pml4_t[510] = ((uint64_t)ker_cr3) | RW_KERNEL_FLAGS;   

	// Kernal Memory Mapping 
	// Mappings for virtual address range [0xFFFFFFFF80200000, 0xFFFFFFFF80406000]
	// to physical address range [0x200000, 0x406000]
	// 2 MB for Kernal + 6 Pages for PML4, PDPE, PDE, PTE(3) tables

	init_map_virt_phys_addr(0x0, 0x0, num_pages, ker_pml4_t, 1); //Allowed user access
	
	init_map_virt_phys_addr(kernmem, physbase, num_pages - physbase/PAGE_SIZE, ker_pml4_t, 0); //Only kernel pages

	// Use existing Video address mapping: Virtual memory 0xFFFFFFFF800B8000 to Physical memory 0xB8000

	init_map_virt_phys_addr(0xFFFFFFFF800B8000, 0xB8000, 1, ker_pml4_t,0);
	//init_map_virt_phys_addr(0xFFFFFFFF800B8000, 0xB8000, 1);

	LOAD_CR3((uint64_t)ker_cr3);
	kprintf("CR3 value: %p %p", (uint64_t)ker_cr3, (uint64_t)kernmem);
	// Set CR3 register to address of PML4 table

	// Set value of top virtual address
	set_highest_virtaddr(kernmem + (num_pages * PAGE_SIZE));

	// Setting available free memory for kmalloc() to zero
	//init_kmalloc();
}

void load_cr3_user() {

	uint64_t *ker_cr3_u = (uint64_t *)PADDR(kmalloc(1));
	//uint64_t *ker_pml4_t_u = (uint64_t *) get_highest_virtaddr();
	uint64_t *ker_pml4_t_u = (uint64_t *)VADDR(ker_cr3_u);
	set_highest_virtaddr((uint64_t)ker_pml4_t_u + PAGE_SIZE);
	uint64_t add = (uint64_t)ker_pml4_t_u << 16 >> 55 << 3;
	kprintf("Tadd: %p %p %p\n", add, ker_pml4_t_u, add | 0xFFFFFF7FBFDFE000UL);
	//while(1);
	init_map_virt_phys_addr((uint64_t)ker_pml4_t_u, (uint64_t)ker_cr3_u, 1, (uint64_t *)ker_pml4_t_u, 1); 
	ker_pml4_t_u[511] = ker_pml4_t[511];
	kprintf("User PML4: %p %p %p %p\n", ker_cr3_u, ker_pml4_t_u, ker_pml4_t_u[511], ker_pml4_t[511]);
	//LOAD_CR3((uint64_t)ker_cr3_u);
}
