#ifndef _PTOPS_H
#define _PTOPS_H

#include <sys/ptmgr.h>
#include <sys/defs.h>

void pte_entry_add_attr(pte_entry *e, uint64_t attr);

void pte_entry_del_attr(pte_entry *e, uint64_t attr);

void pte_entry_set_frame(pte_entry *e, uint64_t phy_addr);

uint64_t pte_entry_get_pfn(pte_entry e);

bool pte_entry_is_writable(pte_entry e);

bool pte_entry_is_present(pte_entry e);

bool pte_entry_is_user(pte_entry e);

void pde_entry_add_attr(pde_entry *e, uint64_t attr);

void pde_entry_del_attr(pte_entry *e, uint64_t attr);

void pde_entry_set_frame(pde_entry *e, uint64_t phy_addr);

uint64_t pde_entry_get_pfn(pde_entry e);

bool pde_entry_is_writable(pde_entry e);

bool pde_entry_is_present(pde_entry e);

bool pde_entry_is_user(pde_entry e);

void pdp_entry_add_attr(pdp_entry *e, uint64_t attr);

void pdp_entry_del_attr(pdp_entry *e, uint64_t attr);

void pdp_entry_set_frame(pdp_entry *e, uint64_t phy_addr);

uint64_t pdp_entry_get_pfn(pdp_entry e);

bool pdp_entry_is_writable(pdp_entry e);

bool pdp_entry_is_present(pdp_entry e);

bool pdp_entry_is_user(pdp_entry e);

void pml4_entry_add_attr(pml4_entry *e, uint64_t attr);

void pml4_entry_del_attr(pml4_entry *e, uint64_t attr);

void pml4_entry_set_frame(pml4_entry *e, uint64_t phy_addr);

uint64_t pml4_entry_get_pfn(pml4_entry e);

bool pml4_entry_is_writable(pml4_entry e);

bool pml4_entry_is_present(pml4_entry e);

bool pml4_entry_is_user(pml4_entry e);

#endif
