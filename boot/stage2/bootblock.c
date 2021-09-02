#include <string.h>
#include <data.h>
#include <gui.h>

unsigned int *cof_bootblock()
{
	unsigned int *mem = (unsigned int *)0x100000;
	unsigned char *boot_dv = (unsigned char*)0x40000;

	memset(mem,0,0x1000); //4 KiB

	// Boot Device Info
	// BIOSes Get Device Parameters (INT 13h FN 48h)
	mem[0] = dv_uid;
	mem[1] = dv_num;
	
	// Device Parameter Table Extension (DPTE)
	// Pointer to the Device Parameter Table Extension (DPTE), flat 32-bits.
	unsigned char *tmp =(unsigned char*) (16 * ((*(unsigned int*)(boot_dv+26)) >>16 &0xffff) +\
	((*(unsigned int*)(boot_dv+26)) &0xffff));
	
	// 16
	mem[4] = (unsigned int)tmp;
	
	// 32
	// Memory map
	mem[8] = PAGING_ADDR;
	mem[9] = PAGING_ADDR + 0x1000;
	mem[10] = PAGING_ADDR + 0x2000;
	mem[11] = PAGING_ADDR + 0x3000; 

	// 48
	// Copia parametro GUI
	memcpy((void*)((char*)mem + 0x40),gui,sizeof(gui_t));
	


	// Physical Memoey Map (INT 0x15 AX,E820h)
	// 8 bytes RAM SIZE 
	
	return mem;
}
