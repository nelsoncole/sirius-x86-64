#include <paging.h>
#include <string.h>
#include <gui.h>
#include <stdio.h>
#include <msr.h>
#include <io.h>

#include <data.h>

#define GUI_VIRTUAL_BUFFER 0x7800000

pml4_table_t *pml4e = (pml4_table_t *)PAGING_ADDR;
pae_page_directory_pointer_table_t *pdpte = (pae_page_directory_pointer_table_t*) (PAGING_ADDR + 0x1000);
pae_page_directory_t *pae_pde = (pae_page_directory_t*) (PAGING_ADDR + 0x2000);
pae_page_table_t *pae_pte = (pae_page_table_t*) (PAGING_ADDR + 0x3000);

void load_pae_page_directory_pointer_table(pae_page_directory_pointer_table_t  *phy_addr)
{

	__asm__ __volatile__("movl %k0,%%cr3"::"r"(phy_addr));
	
	wait(120); // Wait Translation Lookaside Buffer (TLB)
}

void load_pml4_table(pml4_table_t *phy_addr)
{
	__asm__ __volatile__("movl %0,%%cr3"::"r"(phy_addr));
}

void enable_pae()
{

	// PAE and PGE
	__asm__ __volatile__("movl %%cr4,%%eax;"
			     "orl $0x20,%%eax;"
			     "movl %%eax,%%cr4"::);


}

void page_enable()
{	__asm__ __volatile__("movl %%cr0,%%eax;"
			     "orl $0x80000000,%%eax;"
			     "movl %%eax,%%cr0"::);
}


void page_install(void)
{

	pae_page_directory_t *pde = pae_pde;
	pae_page_table_t *pte = pae_pte;
	unsigned long addr;
	int i;
	//PAE PTE
	memset(pte,0,512*sizeof(pae_page_table_t)*64);
	
	//120 MiB
	addr = 0;
	for(i=0;i < 512*60; i++) {
		
		pte->p = 1;
		pte->rw = 1;
		pte->frames = (addr >>12) &0xfffffffff;
		pte++;
		
		addr += 0x1000;
	}
	
	
	// Linear Frame BUffer 8 MiB
	addr = (unsigned long)gui->frame_buffer;
	for(i=0;i < 512 * 4; i++) {
		pte->p = 1;
		pte->rw = 1;
		//pte->us = 1;
		pte->frames = (addr >>12) &0xfffffffff;
		pte++;
		
		addr += 0x1000;
	}
	
	memset(pde,0,512*sizeof(pae_page_directory_t));
	addr = (unsigned long)pae_pte;
	for(i=0;i < 64; i++) {
		
		pde->p = 1;
		pde->rw = 1;
		//if(i > 59) pde->us = 1;
		pde->phy_addr_pt = (addr >>12) &0xfffffffff;
		
		addr +=0x1000;
		pde++;
	}
	
	
	//
	memset(pdpte,0,512*sizeof(pae_page_directory_pointer_table_t));
	pdpte->p = 1;
	pdpte->rw = 1;
	addr = (unsigned long) pae_pde;
	pdpte->phy_addr_pd = (addr >> 12) &0xfffffffff;
		
	//
	memset(pml4e,0,512*sizeof(pae_page_directory_pointer_table_t));
	pml4e->p = 1;
	pml4e->rw = 1;
	pml4e->us = 1;
	addr = (unsigned long) pdpte;
	pml4e->phy_addr_pdpt = (addr >> 12) &0xfffffffff;
	
	
	
	__asm__ __volatile__("cli");
	
	
	enable_pae();
	load_pml4_table(pml4e);
	setmsr(0xC0000080 , 0x100, 0);
	page_enable();
	
	wait(400);
	
	// Update gui->virtual_buffer
	gui->virtual_buffer = GUI_VIRTUAL_BUFFER;
	
}
