#include <paging.h>

pml4_table_t *pml4e = 0;
pae_page_directory_pointer_table_t *pdpte = 0;
pae_page_directory_t *pae_pde = 0;
pae_page_table_t *pae_pte = 0;


void flush_tlb()
{
	unsigned long cr3;
	__asm__ __volatile__("mov %%cr3, %0":"=r"(cr3):);
	load_pml4_table((pml4_table_t *)cr3);
	for(int i=200; i > 0;i--) __asm__ __volatile__("nop;");
}


void load_pae_page_directory_pointer_table(pae_page_directory_pointer_table_t  *phy_addr)
{

	__asm__ __volatile__("movq %0,%%cr3"::"r"(phy_addr));
	
 	// Wait 120ns Translation Lookaside Buffer (TLB)
}

void load_pml4_table(pml4_table_t *phy_addr)
{
	__asm__ __volatile__("movq %0,%%cr3"::"r"(phy_addr));
}

void enable_pae()
{

	// PAE and PGE
	__asm__ __volatile__("movq %%cr4,%%rax;"
			     "orl $0x20,%%eax;"
			     "movq %%rax,%%cr4"::);


}

void page_enable()
{	__asm__ __volatile__("movq %%cr0,%%rax;"
			     "orl $0x80000000,%%eax;"
			     "movq %%rax,%%cr0"::);
}


void page_install(unsigned long entry_pointer_info)
{
	unsigned long paging_addr = *(unsigned int*)(entry_pointer_info + 0x20);

	pml4e = (pml4_table_t *) paging_addr;
	pdpte = (pae_page_directory_pointer_table_t*) (paging_addr + 0x1000);
	pae_pde = (pae_page_directory_t*) (paging_addr + 0x2000);
	pae_pte = (pae_page_table_t*) (paging_addr + 0x3000);
	
}
