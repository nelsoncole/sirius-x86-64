#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gui.h>
#include <gdt.h>
#include <idt.h>
#include <pci.h>
#include <storage.h>
#include <fs.h>
#include <cpuid.h>
#include <paging.h>
#include <data.h>
#include <irq.h>
#include <ps2.h>
#include <ehci.h>

extern unsigned int *cof_bootblock();
extern int i965();
extern int file_sector_count;
void main()
{

	initialize_heap(DEF_HEAP_START);
	initialize_gui();
	
	printf("===============================================\n");
	
	
	char cpu[64];
	cpuid_processor_brand(cpu);
	printf("%s\n",cpu);
	printf("===============================================\n");
	
	printf("VBE video mode: %dx%dx%d\n",gui->width,gui->height,gui->bpp);
	printf("===============================================\n");
	
	printf("GDT and IDT install\n");
	
	gdt_install();
	idt_install();

	printf("===============================================\n");
	

	//keyboard_install();
	//mouse_install();
	sti();
	
	printf("===============================================\n");
	ata_initialize();
	
	printf("===============================================\n");
	pci_get_info(0,2);
	
	// TODO: 
	dv_uid = -1;
	dv_num = 0;
	
	/*printf("===============================================\n");
	
	ehci_init(); // TODO USB */
	
	printf("===============================================\n");
	
	printf("Reading the kernel.bin\n");
	
	if(!file_sector_count) {
	
		FILE *f = fopen("kernel.bin","r");
		if(!f) {
	
			printf("error read kernel.bin\n");
			for(;;);
		}
	
		fseek(f,0,SEEK_END);
		int size = ftell(f);
		rewind(f);
		
		fread ((void*)0x400000, 1, size, f);
		
		fclose(f);
	} else {
	
		
		memcpy((char*)0x400000, (const char*)0x50000, file_sector_count * (16*512) );
	
	}
	
	printf("===============================================\n");
	
	// Verificar suport x86_64
	unsigned int a,d;
	cpuid(0x80000001, &a,&d);
	
	// Test LM-bit
	if(!(d >> 29 &1)) {
			
			puts("x86_64 hardware not supported\n");
			for(;;);
	
	}
	
	puts("Enable 4-level paging\n");
	page_install();

	printf("===============================================\n");
	
	unsigned int bootblock = (unsigned int) cof_bootblock();
	gdt_execute_long_mode(0x401000, bootblock);
}
