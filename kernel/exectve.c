#include <thread.h>
#include <paging.h>
#include <stdio.h>
#include <string.h>

#include <header.h>
#include <mm.h>
#include <stdlib.h>

#include <kernel.h>
#include <data.h>

#define APP_STAK_SIZE (0x1000*32) // 128 KiB
#define APP_ARG_SIZE  (0x1000*16) // 64 KiB

#define APP_ARG_LEN 	256
#define APP_ARG_COUNT 	128

extern int load_elf64_segment(void *bf);
extern int load_elf64_header(void *bf, program_header_t *phdr);
unsigned long exectve(int argc, char **argv, char *pwd, void *bf)
{


    unsigned long phy;
    
    program_header_t phdr;
    if(load_elf64_header(bf,&phdr)){
        return 0;
    }

    phdr.stack = phdr.end + APP_STAK_SIZE + APP_ARG_SIZE;

	
	int physize = (phdr.stack - phdr.start);
	int pagesize = (physize/0x1000);
	if(physize%0x1000) pagesize += 0x1;
	int tbs = pagesize/512;
	if(pagesize % 512) tbs ++;

	pml4_table_t *_pml4e = 0;
	pae_page_directory_pointer_table_t *_pdpte = 0;
	pae_page_directory_t *pde = 0;
	pae_page_table_t *pte = 0;
	
	alloc_pages(0, 1, (unsigned long *)&_pml4e);
	memset(_pml4e, 0, sizeof(pml4_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&_pdpte);
	memset(_pdpte, 0, sizeof(pae_page_directory_pointer_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&pde);
	memset(pde, 0, sizeof(pae_page_directory_t)*512);
	
	alloc_pages(0, tbs, (unsigned long *)&pte);
	memset(pte, 0, sizeof(pae_page_table_t)*512*tbs);
	
	
	// copiar as paginas do kernel
	memcpy(_pml4e, pml4e, sizeof(pml4_table_t)*1);
	//memcpy(_pdpte, pdpte, sizeof(pae_page_directory_pointer_table_t)*4);
	
	
	pae_page_table_t *_pte = pte;
	for(int i=0;i < pagesize; i++) {
		
		unsigned long frame = alloc_frame();
		
		_pte->p = 1;
		_pte->rw = 1;
		_pte->us = 1;
		_pte->frames = (frame >>12) &0xffffffffffff;
		_pte++;
	}
	
	pae_page_directory_t *_pde = pde;
	unsigned long virt = (unsigned long)pte;
	for(int i=0;i < tbs; i++) {
		phy = get_phy_addr(virt);
		
		_pde->p = 1;
		_pde->rw = 1;
		_pde->us = 1;
		_pde->phy_addr_pt = (phy >>12) &0xffffffffffff;
		
		virt += 0x1000;		
		_pde++;

	}
	
	phy = get_phy_addr ((unsigned long)pde);
	_pdpte[0].p = 1;
	_pdpte[0].rw = 1;
	_pdpte[0].us = 1;
	_pdpte[0].phy_addr_pd = (phy >> 12) &0xffffffffffff;
	
	phy = get_phy_addr ((unsigned long)_pdpte);
	_pml4e[1].p = 1;
	_pml4e[1].rw = 1;
	_pml4e[1].us = 1;
	_pml4e[1].phy_addr_pdpt = (phy >> 12) &0xffffffffffff;
	phy = get_phy_addr ((unsigned long)_pml4e);
	
	
	unsigned long cr3;
	__asm__ __volatile__("mov %%cr3, %0":"=r"(cr3):);
	
	
	load_pml4_table((pml4_table_t *)phy);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");

	memset((char*)0x8000000000, 0, pagesize*0x1000 ); //TODO
    load_elf64_segment(bf);
	
	
	unsigned long argv_ptr = phdr.end;
	char *_pwd = (char*) argv_ptr;
	argv_ptr += 0x1000;
	char **arg = (char**) argv_ptr;
	argv_ptr += 0x1000;
	for(int i =0; i < APP_ARG_COUNT; i ++) arg[i] = (char*) (argv_ptr + (i*APP_ARG_LEN));

	
	if(pwd){
		strcpy(_pwd,pwd);
	}
	
	if(argv) {
		for(int i=0; i < argc; i ++) strcpy(arg[i],argv[i]);
	}

	
	load_pml4_table((pml4_table_t *)cr3);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");

	

	THREAD *thread = create_thread((void*)phdr.entry, phdr.stack,
	phy, (unsigned long)user, 0x11, argc, arg, _pwd);


	thread->pml4e_addr = (unsigned long) _pml4e;
	thread->pdpte_addr = (unsigned long) _pdpte;
	thread->pde_addr = (unsigned long) pde;
	thread->pte_addr = (unsigned long) pte;

    
	thread->frame_size = pagesize;

	return(thread->pid );
}

unsigned long exectve_child(int argc, char **argv, char *pwd, void *bf, THREAD *thread )
{


    unsigned long phy;
    
    program_header_t phdr;
    if(load_elf64_header(bf,&phdr)){
        return 0;
    }

    phdr.stack = phdr.end + APP_STAK_SIZE + APP_ARG_SIZE;

	int physize = (phdr.stack - phdr.start);
	int pagesize = (physize/0x1000);
	if(physize%0x1000) pagesize += 0x1;
	int tbs = pagesize/512;
	if(pagesize % 512) tbs ++;

	pml4_table_t *_pml4e = 0;
	pae_page_directory_pointer_table_t *_pdpte = 0;
	pae_page_directory_t *pde = 0;
	pae_page_table_t *pte = 0;
	
	alloc_pages(0, 1, (unsigned long *)&_pml4e);
	memset(_pml4e, 0, sizeof(pml4_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&_pdpte);
	memset(_pdpte, 0, sizeof(pae_page_directory_pointer_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&pde);
	memset(pde, 0, sizeof(pae_page_directory_t)*512);
	
	alloc_pages(0, tbs, (unsigned long *)&pte);
	memset(pte, 0, sizeof(pae_page_table_t)*512*tbs);
	
	
	// copiar as paginas do kernel
	memcpy(_pml4e, pml4e, sizeof(pml4_table_t)*1);
	//memcpy(_pdpte, pdpte, sizeof(pae_page_directory_pointer_table_t)*4);
	
	
	pae_page_table_t *_pte = pte;
	for(int i=0;i < pagesize; i++) {
		
		unsigned long frame = alloc_frame();
		
		_pte->p = 1;
		_pte->rw = 1;
		_pte->us = 1;
		_pte->frames = (frame >>12) &0xffffffffffff;
		_pte++;
	}
	
	pae_page_directory_t *_pde = pde;
	unsigned long virt = (unsigned long)pte;
	for(int i=0;i < tbs; i++) {
		phy = get_phy_addr(virt);
		
		_pde->p = 1;
		_pde->rw = 1;
		_pde->us = 1;
		_pde->phy_addr_pt = (phy >>12) &0xffffffffffff;
		
		virt += 0x1000;		
		_pde++;

	}
	
	phy = get_phy_addr ((unsigned long)pde);
	_pdpte[0].p = 1;
	_pdpte[0].rw = 1;
	_pdpte[0].us = 1;
	_pdpte[0].phy_addr_pd = (phy >> 12) &0xffffffffffff;
	
	phy = get_phy_addr ((unsigned long)_pdpte);
	_pml4e[1].p = 1;
	_pml4e[1].rw = 1;
	_pml4e[1].us = 1;
	_pml4e[1].phy_addr_pdpt = (phy >> 12) &0xffffffffffff;
	phy = get_phy_addr ((unsigned long)_pml4e);
	
	
	unsigned long cr3;
	__asm__ __volatile__("mov %%cr3, %0":"=r"(cr3):);
	
	
	load_pml4_table((pml4_table_t *)phy);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");
	

	memset((char*)0x8000000000, 0, pagesize*0x1000 ); //TODO
	load_elf64_segment(bf);
	
	
	
	unsigned long argv_ptr = ((0x8000000000 + physize) - (APP_STAK_SIZE + APP_ARG_SIZE));
	char *_pwd = (char*) argv_ptr;
	argv_ptr += 0x1000;
	char **arg = (char**) argv_ptr;
	argv_ptr += 0x1000;
	for(int i =0; i < APP_ARG_COUNT; i ++) arg[i] = (char*) (argv_ptr + (i*APP_ARG_LEN));

	if(pwd){
		strcpy(_pwd,pwd);
	}
	
	if(argv) {
		for(int i=0; i < argc; i ++) strcpy(arg[i],argv[i]);
	}
	
	load_pml4_table((pml4_table_t *)cr3);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");

	

	THREAD *thread2 = create_thread_child((void*)phdr.entry, phdr.stack,
	phy, (unsigned long)user, 0x1, argc, arg, _pwd, thread);
	
	thread2->pml4e_addr = (unsigned long) _pml4e;
	thread2->pdpte_addr = (unsigned long) _pdpte;
	thread2->pde_addr = (unsigned long) pde;
	thread2->pte_addr = (unsigned long) pte;
	
	thread2->frame_size = pagesize;

	return( thread2->pid );
}

/*

unsigned long ___exectve(int argc, char **argv, char *pwd, FILE *fp)
{
    unsigned long phy;
	int no = 0;
	HEADER *header = (HEADER*)malloc(sizeof(HEADER));
	
	fseek(fp,0x1000,SEEK_SET);
	for(int i=0; i < 0x1000; i  += 8){
	
		fread (header, 1, 8, fp);
		
		if (memcmp("_ cole _",header->magic,8) == 0) {
		
			fread ((char*)header + 8, 1, sizeof(HEADER) - 8, fp);
			no++;
			break;
		}
	
	}
	
	if(!no) {
		free(header);
		return 0;
	}
	
	int physize = (header->end -  (header->start - 0x1000)) + APP_STAK_SIZE + APP_ARG_SIZE;
	int pagesize = (physize/0x1000);
	if(physize%0x1000) pagesize += 0x1;
	int tbs = pagesize/512;
	if(pagesize % 512) tbs ++;

	pml4_table_t *_pml4e = 0;
	pae_page_directory_pointer_table_t *_pdpte = 0;
	pae_page_directory_t *pde = 0;
	pae_page_table_t *pte = 0;
	
	alloc_pages(0, 1, (unsigned long *)&_pml4e);
	memset(_pml4e, 0, sizeof(pml4_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&_pdpte);
	memset(_pdpte, 0, sizeof(pae_page_directory_pointer_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&pde);
	memset(pde, 0, sizeof(pae_page_directory_t)*512);
	
	alloc_pages(0, tbs, (unsigned long *)&pte);
	memset(pte, 0, sizeof(pae_page_table_t)*512*tbs);
	
	
	// copiar as paginas do kernel
	memcpy(_pml4e, pml4e, sizeof(pml4_table_t)*1);
	//memcpy(_pdpte, pdpte, sizeof(pae_page_directory_pointer_table_t)*4);
	
	
	pae_page_table_t *_pte = pte;
	for(int i=0;i < pagesize; i++) {
		
		unsigned long frame = alloc_frame();
		
		_pte->p = 1;
		_pte->rw = 1;
		_pte->us = 1;
		_pte->frames = (frame >>12) &0xffffffffffff;
		_pte++;
	}
	
	pae_page_directory_t *_pde = pde;
	unsigned long virt = (unsigned long)pte;
	for(int i=0;i < tbs; i++) {
		phy = get_phy_addr(virt);
		
		_pde->p = 1;
		_pde->rw = 1;
		_pde->us = 1;
		_pde->phy_addr_pt = (phy >>12) &0xffffffffffff;
		
		virt += 0x1000;		
		_pde++;

	}
	
	phy = get_phy_addr ((unsigned long)pde);
	_pdpte[0].p = 1;
	_pdpte[0].rw = 1;
	_pdpte[0].us = 1;
	_pdpte[0].phy_addr_pd = (phy >> 12) &0xffffffffffff;
	
	phy = get_phy_addr ((unsigned long)_pdpte);
	_pml4e[1].p = 1;
	_pml4e[1].rw = 1;
	_pml4e[1].us = 1;
	_pml4e[1].phy_addr_pdpt = (phy >> 12) &0xffffffffffff;
	phy = get_phy_addr ((unsigned long)_pml4e);
	
	
	unsigned long cr3;
	__asm__ __volatile__("mov %%cr3, %0":"=r"(cr3):);
	
	
	load_pml4_table((pml4_table_t *)phy);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");
	
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	
	memset((char*)0x8000000000, 0, pagesize*0x1000 ); //TODO
	
	fread ((char*)0x8000000000, 1, size, fp);
	
	
	unsigned long argv_ptr = ((0x8000000000 + physize) - (APP_STAK_SIZE + APP_ARG_SIZE));
	char *_pwd = (char*) argv_ptr;
	argv_ptr += 0x1000;
	char **arg = (char**) argv_ptr;
	argv_ptr += 0x1000;
	for(int i =0; i < APP_ARG_COUNT; i ++) arg[i] = (char*) (argv_ptr + (i*APP_ARG_LEN));

	
	if(pwd){
		strcpy(_pwd,pwd);
	}
	
	if(argv) {
		for(int i=0; i < argc; i ++) strcpy(arg[i],argv[i]);
	}
	
	load_pml4_table((pml4_table_t *)cr3);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");

	

	THREAD *thread = create_thread((void*)header->start,
	(unsigned long) (0x8000000000 + physize),
	phy, (unsigned long)user, 0x11, argc, arg, _pwd);
	
	thread->pml4e_addr = (unsigned long) _pml4e;
	thread->pdpte_addr = (unsigned long) _pdpte;
	thread->pde_addr = (unsigned long) pde;
	thread->pte_addr = (unsigned long) pte;
	
	thread->frame_size = pagesize;

	free(header);
	return(thread->pid );
}


unsigned long __exectve_child(int argc, char **argv, char *pwd, FILE *fp, THREAD *thread )
{

	unsigned long phy;
	int no = 0;
	HEADER *header = (HEADER*)malloc(sizeof(HEADER));
	
	fseek(fp,0x1000,SEEK_SET);
	for(int i=0; i < 0x1000; i  += 8){
	
		fread (header, 1, 8, fp);
		
		if (memcmp("_ cole _",header->magic,8) == 0) {
		
			fread ((char*)header + 8, 1, sizeof(HEADER) - 8, fp);
			no++;
			break;
		}
	
	}
	
	if(!no) {
		free(header);
		return 0;
	}
	
	int physize = (header->end -  (header->start - 0x1000)) + APP_STAK_SIZE + APP_ARG_SIZE;
	int pagesize = (physize/0x1000);
	if(physize%0x1000) pagesize += 0x1;
	int tbs = pagesize/512;
	if(pagesize % 512) tbs ++;

	pml4_table_t *_pml4e = 0;
	pae_page_directory_pointer_table_t *_pdpte = 0;
	pae_page_directory_t *pde = 0;
	pae_page_table_t *pte = 0;
	
	alloc_pages(0, 1, (unsigned long *)&_pml4e);
	memset(_pml4e, 0, sizeof(pml4_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&_pdpte);
	memset(_pdpte, 0, sizeof(pae_page_directory_pointer_table_t)*512);
	
	alloc_pages(0, 1, (unsigned long *)&pde);
	memset(pde, 0, sizeof(pae_page_directory_t)*512);
	
	alloc_pages(0, tbs, (unsigned long *)&pte);
	memset(pte, 0, sizeof(pae_page_table_t)*512*tbs);
	
	
	// copiar as paginas do kernel
	memcpy(_pml4e, pml4e, sizeof(pml4_table_t)*1);
	//memcpy(_pdpte, pdpte, sizeof(pae_page_directory_pointer_table_t)*4);
	
	
	pae_page_table_t *_pte = pte;
	for(int i=0;i < pagesize; i++) {
		
		unsigned long frame = alloc_frame();
		
		_pte->p = 1;
		_pte->rw = 1;
		_pte->us = 1;
		_pte->frames = (frame >>12) &0xffffffffffff;
		_pte++;
	}
	
	pae_page_directory_t *_pde = pde;
	unsigned long virt = (unsigned long)pte;
	for(int i=0;i < tbs; i++) {
		phy = get_phy_addr(virt);
		
		_pde->p = 1;
		_pde->rw = 1;
		_pde->us = 1;
		_pde->phy_addr_pt = (phy >>12) &0xffffffffffff;
		
		virt += 0x1000;		
		_pde++;

	}
	
	phy = get_phy_addr ((unsigned long)pde);
	_pdpte[0].p = 1;
	_pdpte[0].rw = 1;
	_pdpte[0].us = 1;
	_pdpte[0].phy_addr_pd = (phy >> 12) &0xffffffffffff;
	
	phy = get_phy_addr ((unsigned long)_pdpte);
	_pml4e[1].p = 1;
	_pml4e[1].rw = 1;
	_pml4e[1].us = 1;
	_pml4e[1].phy_addr_pdpt = (phy >> 12) &0xffffffffffff;
	phy = get_phy_addr ((unsigned long)_pml4e);
	
	
	unsigned long cr3;
	__asm__ __volatile__("mov %%cr3, %0":"=r"(cr3):);
	
	
	load_pml4_table((pml4_table_t *)phy);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");
	
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	
	memset((char*)0x8000000000, 0, pagesize*0x1000 ); //TODO
	
	fread ((char*)0x8000000000, 1, size, fp);
	
	
	
	unsigned long argv_ptr = ((0x8000000000 + physize) - (APP_STAK_SIZE + APP_ARG_SIZE));
	char *_pwd = (char*) argv_ptr;
	argv_ptr += 0x1000;
	char **arg = (char**) argv_ptr;
	argv_ptr += 0x1000;
	for(int i =0; i < APP_ARG_COUNT; i ++) arg[i] = (char*) (argv_ptr + (i*APP_ARG_LEN));

	if(pwd){
		strcpy(_pwd,pwd);
	}
	
	if(argv) {
		for(int i=0; i < argc; i ++) strcpy(arg[i],argv[i]);
	}
	
	load_pml4_table((pml4_table_t *)cr3);
	for(int i=200; i > 0;i--) __asm__ __volatile__("pause");

	

	THREAD *thread_ = create_thread_child((void*)header->start,
	(unsigned long)(0x8000000000 + physize),
	phy, (unsigned long)user, 0x1, argc, arg, _pwd, thread);
	
	thread_->pml4e_addr = (unsigned long) _pml4e;
	thread_->pdpte_addr = (unsigned long) _pdpte;
	thread_->pde_addr = (unsigned long) pde;
	thread_->pte_addr = (unsigned long) pte;
	
	thread_->frame_size = pagesize;

	free(header);
	return( thread_->pid );
} 
*/
