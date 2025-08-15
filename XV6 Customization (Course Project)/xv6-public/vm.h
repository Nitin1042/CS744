#ifndef VM_H
#define VM_H

pte_t * get_walkpgdir(pde_t *pgdir, const void *va, int alloc);
void lazy_page_fault(uint addr);
#endif 
