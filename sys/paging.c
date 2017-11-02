#include <sys/mem.h>
#include <sys/kprintf.h>

#define RW_KERNEL_FLAGS 3UL
#define VADDR(PADDR) (KERNEL_VADDR + ((uint64_t)PADDR >> (uint64_t)12 << (uint64_t)12))
#define PAGESIZE 4096
#define ENTRIES_PER_PTE 512
#define IS_PRESENT_PAGE(addr) ((uint64_t)addr & 1)

static uint64_t* alloc_pte(uint64_t *pde_table, int pde_off)
{
	uint64_t *pte_table = kmalloc(1);
    pde_table[pde_off] = ((uint64_t)pte_table << (uint64_t)12) | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pte_table);
} 

static uint64_t* alloc_pde(uint64_t *pdpe_table, int pdpe_off)
{
    uint64_t* pde_table = kmalloc(1);
    pdpe_table[pdpe_off] = ((uint64_t)pde_table << (uint64_t)12) | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pde_table);
}

static uint64_t* alloc_pdpe(uint64_t *pml4_table, int pml4_off)
{
    uint64_t* pdpe_table = kmalloc(1);
    pml4_table[pml4_off] = ((uint64_t)pdpe_table << (uint64_t)12) | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pdpe_table);
}

static uint64_t *ker_pml4_t;
static uint64_t *ker_cr3;
uint64_t* get_ker_pml4_t()
{
    return ker_pml4_t;
}

static void init_map_virt_phys_addr(uint64_t vaddr, uint64_t paddr, uint64_t no_of_pages)
{
    uint64_t *pdpe_table = NULL, *pde_table = NULL, *pte_table = NULL;

    int i, j, k, phys_addr, pde_off, pdpe_off, pml4_off , pte_off; 

    pte_off  = (vaddr >> 12) & 0x1FF;
    pde_off  = (vaddr >> 21) & 0x1FF;
    pdpe_off = (vaddr >> 30) & 0x1FF;
    pml4_off = (vaddr >> 39) & 0x1FF;

    // kprintf(" $$ NEW MAPPING $$ OFF => %d %d %d %d ", pml4_off, pdpe_off, pde_off, pte_off);

    phys_addr = (uint64_t) *(ker_pml4_t + pml4_off);
    // kprintf("%p",phys_addr);
    if (IS_PRESENT_PAGE(phys_addr)) {
        pdpe_table =(uint64_t*) VADDR(phys_addr); 

        phys_addr = (uint64_t) *(pdpe_table + pdpe_off);
        if (IS_PRESENT_PAGE(phys_addr)) {
            pde_table =(uint64_t*) VADDR(phys_addr); 

            phys_addr  = (uint64_t) *(pde_table + pde_off);
            if (IS_PRESENT_PAGE(phys_addr)) {
                pte_table =(uint64_t*) VADDR(phys_addr); 
            } else {
                pte_table = alloc_pte(pde_table, pde_off);
            }
        } else {
            pde_table = alloc_pde(pdpe_table, pdpe_off);
            pte_table = alloc_pte(pde_table, pde_off);
        }
    } else {
        pdpe_table = alloc_pdpe(ker_pml4_t, pml4_off);
        pde_table = alloc_pde(pdpe_table, pdpe_off);
        pte_table = alloc_pte(pde_table, pde_off);
    }

    phys_addr = paddr;  

    if (no_of_pages + pte_off <= ENTRIES_PER_PTE) {
        for (i = pte_off; i < (pte_off + no_of_pages); i++) {
            pte_table[i] = phys_addr | RW_KERNEL_FLAGS; 
            phys_addr += PAGESIZE;
        }
        // kprintf(" SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", no_of_pages, pdpe_table , pde_table, pte_table);
    } else {
        int lno_of_pages = no_of_pages, no_of_pte_t;

        // kprintf(" SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
        for ( i = pte_off ; i < ENTRIES_PER_PTE; i++) {
            pte_table[i] = phys_addr | RW_KERNEL_FLAGS;
            phys_addr += PAGESIZE;
        }
        lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE - pte_off);
        no_of_pte_t = lno_of_pages/ENTRIES_PER_PTE;

        for (j = 1; j <= no_of_pte_t; j++) {   
            pte_table = alloc_pte(pde_table, pde_off+j);
            // kprintf(" SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
            for(k = 0; k < ENTRIES_PER_PTE; k++ ) { 
                pte_table[k] = phys_addr | RW_KERNEL_FLAGS;
                phys_addr += PAGESIZE;
            }
        }
        lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE * pte_off);
        pte_table = alloc_pte(pde_table, pde_off+j);
        
        // kprintf(" SIZE = %d PDPE Address %p, PDE Address %p, PTE Address %p", lno_of_pages, pdpe_table ,pde_table, pte_table);
        for(k = 0; k < lno_of_pages; k++ ) { 
            pte_table[k] = phys_addr | RW_KERNEL_FLAGS;
            phys_addr += PAGESIZE;
        }
    }
}

void LOAD_CR3(uint64_t addr) {

        __asm__ volatile ( "mov %0, %%cr3;"
                         :: "r" (addr));
}

void init_paging(uint64_t kernmem, uint64_t physbase, uint64_t no_of_pages)
{
    // Allocate free memory for PML4 table 
    ker_cr3 = kmalloc(1);

    ker_pml4_t = (uint64_t*) VADDR(ker_cr3);
    //kprintf("\tKernel PML4t:%p", ker_pml4_t);

    ker_pml4_t[510] = ((uint64_t)ker_cr3 << (uint64_t)12) | RW_KERNEL_FLAGS;   
    
    // Kernal Memory Mapping 
    // Mappings for virtual address range [0xFFFFFFFF80200000, 0xFFFFFFFF80406000]
    // to physical address range [0x200000, 0x406000]
    // 2 MB for Kernal + 6 Pages for PML4, PDPE, PDE, PTE(3) tables
    init_map_virt_phys_addr(kernmem, physbase, no_of_pages);

    // Use existing Video address mapping: Virtual memory 0xFFFFFFFF800B8000 to Physical memory 0xB8000
    init_map_virt_phys_addr(0xFFFFFFFF800B8000, 0xB8000, 1);
    
    kprintf("CR3 value: %p", (uint64_t)ker_cr3 << (uint64_t)12);
    // Set CR3 register to address of PML4 table
    //LOAD_CR3((uint64_t)ker_cr3 << (uint64_t)12);
    
    // Set value of top virtual address
    //set_top_virtaddr(kernmem + (no_of_pages * PAGESIZE));
           
    // Setting available free memory for kmalloc() to zero
    //init_kmalloc();
}
