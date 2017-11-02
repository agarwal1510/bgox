#include <sys/ptmgr.h>
#include <sys/defs.h>

void pte_entry_add_attr(pte_entry *e, uint64_t attr){
	
	*e |= (attr);
}

void pte_entry_del_attr(pte_entry *e, uint64_t attr){
	
	*e &= ~(attr);
}

void pte_entry_set_frame(pte_entry *e, uint64_t phy_addr){
	
	
	*e = (*e & ~PTE_FRAME) |((uint64_t)phy_addr << (uint64_t)12);

}

uint64_t pte_entry_get_pfn(pte_entry e){

	return e & PTE_FRAME; // Return bits 12 - 48 for Frame Address

}

bool pte_entry_is_writable(pte_entry e){

	return e & PTE_WRITABLE; // Return bits 12 - 48 for Frame Address

}

bool pte_entry_is_present(pte_entry e){

	return e & PTE_PRESENT; // Return bits 12 - 48 for Frame Address

}

bool pte_entry_is_user(pte_entry e){

	return e & PTE_USER; // Return bits 12 - 48 for Frame Address

}

void pde_entry_add_attr(pde_entry *e, uint64_t attr){
	
	*e |= (attr);
}

void pde_entry_del_attr(pde_entry *e, uint64_t attr){
	
	*e &= ~(attr);
}

void pde_entry_set_frame(pde_entry *e, uint64_t phy_addr){
	
//	*e = (*e & 0xffful) | (phy_addr << 12);
	*e = (*e & ~PTE_FRAME) |((uint64_t)phy_addr << (uint64_t)12);
//	*e = (*e & ~PTE_FRAME) |(phy_addr << 12);

}

uint64_t pde_entry_get_pfn(pde_entry e){

	return e & PDE_FRAME; // Return bits 12 - 48 for Frame Address

}

bool pde_entry_is_writable(pde_entry e){

	return e & PDE_WRITABLE; // Return bits 12 - 48 for Frame Address

}

bool pde_entry_is_present(pde_entry e){

	return e & PDE_PRESENT; // Return bits 12 - 48 for Frame Address

}

bool pde_entry_is_user(pde_entry e){

	return e & PDE_USER; // Return bits 12 - 48 for Frame Address

}

void pdp_entry_add_attr(pdp_entry *e, uint64_t attr){

        *e |= (attr);
}

void pdp_entry_del_attr(pdp_entry *e, uint64_t attr){

        *e &= ~(attr);
}

void pdp_entry_set_frame(pdp_entry *e, uint64_t phy_addr){

     //   *e = (*e & 0xffful) | (phy_addr << 12);
	*e = (*e & ~PTE_FRAME) |((uint64_t)phy_addr << (uint64_t)12);
//	*e = (*e & ~PTE_FRAME) |(phy_addr << 12);

}

uint64_t pdp_entry_get_pfn(pdp_entry e){

        return e & PDP_FRAME; // Return bits 12 - 48 for Frame Address

}

bool pdp_entry_is_writable(pdp_entry e){

        return e & PDP_WRITABLE; // Return bits 12 - 48 for Frame Address

}

bool pdp_entry_is_present(pdp_entry e){

        return e & PDP_PRESENT; // Return bits 12 - 48 for Frame Address

}

bool pdp_entry_is_user(pdp_entry e){

        return e & PDP_USER; // Return bits 12 - 48 for Frame Address

}

void pml4_entry_add_attr(pml4_entry *e, uint64_t attr){

        *e |= (attr);
}

void pml4_entry_del_attr(pml4_entry *e, uint64_t attr){

        *e &= ~(attr);
}

void pml4_entry_set_frame(pml4_entry *e, uint64_t phy_addr){

	*e = (*e & ~PTE_FRAME) |((uint64_t)phy_addr << (uint64_t)12);
    //    *e = (*e & 0xffful) | (phy_addr << 12);
	//*e = (*e & ~PTE_FRAME) |(phy_addr << 12);

}

uint64_t pml4_entry_get_pfn(pml4_entry e){

        return e & PML4_FRAME; // Return bits 12 - 48 for Frame Address

}

bool pml4_entry_is_writable(pml4_entry e){

        return e & PML4_WRITABLE; // Return bits 12 - 48 for Frame Address

}

bool pml4_entry_is_present(pml4_entry e){

        return e & PML4_PRESENT; // Return bits 12 - 48 for Frame Address

}

bool pml4_entry_is_user(pml4_entry e){

        return e & PML4_USER; // Return bits 12 - 48 for Frame Address

}
