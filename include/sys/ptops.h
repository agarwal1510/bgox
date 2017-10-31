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

void pde_entry_add_attr(pde_entry *e, uint64_t attr);

void pde_entry_del_attr(pte_entry *e, uint64_t attr);

void pde_entry_set_frame(pde_entry *e, uint64_t phy_addr);

uint64_t pde_entry_get_pfn(pde_entry e);

bool pde_entry_is_writable(pde_entry e);

bool pde_entry_is_present(pde_entry e);

bool pde_entry_is_user(pde_entry e);

#endif
