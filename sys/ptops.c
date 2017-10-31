#include <sys/ptmgr.h>
#include <sys/defs.h>

void pte_entry_add_attr(pte_entry *e, uint64_t attr){
	
	*e |= (attr);
}

void pte_entry_del_attr(pte_entry *e, uint64_t attr){
	
	*e &= ~(attr);
}

void pte_entry_set_frame(pte_entry *e, uint64_t phy_addr){
	
	*e = (*e & 0xffful) | phy_addr;

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

void pde_entry_add_attr(pde_entry *e, uint64_t attr){
	
	*e |= (attr);
}

void pde_entry_del_attr(pte_entry *e, uint64_t attr){
	
	*e &= ~(attr);
}

void pde_entry_set_frame(pde_entry *e, uint64_t phy_addr){
	
	*e = (*e & 0xffful) | phy_addr;

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
