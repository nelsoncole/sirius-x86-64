#include <acpi.h>
#include <io.h>

#include <stdio.h>
#include <string.h>

#include <delay.h>
#include <gdt.h>
#include <idt.h>
#include <paging.h>

extern gdtr_t gdtr[1];
extern idtr_t idtr[1];

extern unsigned long _ap_stack;

extern void screen_refresh();

typedef struct _core {
	unsigned char lapic_ids[256];	// CPU core Local APIC IDs
	unsigned char numcore;		// number of cores detected
	unsigned long lapic;		// pointer to the Local APIC MMIO registers
	unsigned long ioapic;		// pointer to the IO APIC MMIO registers
	unsigned char bspid;		// BSP id
}__attribute__ ((packed)) core_t;

core_t core[1];
extern unsigned long lapicbase;
void ap_startup(int apicid);

void setup_smp() {

	volatile unsigned int *bspdone = (volatile unsigned int *)0x8010;
	volatile unsigned int *aprunning = (volatile unsigned int *) 0x8014; 
	
	int numcore = 0;
	unsigned char *ptr, *ptr2;
	
	printf("ACPI MADT (Multiple APIC Description Table) ... \\\\ \n");
	 
	ACPI_TABLE_MADT *madt = (ACPI_TABLE_MADT*) acpi_probe(rsdt, xsdt, "APIC");
	
	if(!madt) {
		printf("ACPI MADT not found!\n");
		return;
	}
	
	// detect cores
      	ptr = (unsigned char*) madt;	
      	ptr2 = ptr + madt->length;
      	core->lapic = (unsigned long)madt->lapic;
      	
      	for(ptr +=44 ; ptr < ptr2; ptr += ptr[1]) {
        
        	switch(ptr[0]) {
          		case 0: 
          			// found Processor Local APIC
          			if(ptr[4] & 1) core->lapic_ids[numcore++] = ptr[3]; 
          			break;
          		case 1: 
          			// found IOAPIC
          			core->ioapic = (unsigned long)*((unsigned int*)(ptr+4)); 
          			break;
          		case 5: 
          			// found 64 bit LAPIC
          			core->lapic = *((unsigned long*)(ptr+4)); 
          			break;       
        	}
      	}
      
      	core->numcore = numcore;
      	
	__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
	: "=b"(core->bspid) );
      
	printf("Found %d cores, IOAPIC %x, LAPIC %x\nProcessor IDs:", core->numcore, core->ioapic, core->lapic);
	
	for(int i = 0; i < core->numcore; i++)
		printf(" %d", core->lapic_ids[i]);
		
	printf("\nBSP ID: %d",core->bspid);
	
	
	printf("\nInitialize APs... \\\\");
	
	// copy the AP trampoline code to a fixed address in low conventional memory (to address 0x0800:0x0000)
	memcpy((void*)0x8000, (void*)0x40000, 4096); 
	unsigned long apstatck = (unsigned long)_ap_stack;
	
	*aprunning = 0;
	*bspdone = 0;
	
	unsigned long lapic_ptr = (unsigned long) lapicbase;// core->lapic;

	
	for(int i = 0; i < core->numcore; i++) {
		// do not start BSP, that's already running this code
		if(core->lapic_ids[i] == core->bspid) continue;
		
		*(unsigned  int*)(unsigned long)0x8018 = apstatck; // stack
		*(unsigned long*)(unsigned long)0x8020 =(unsigned long)gdtr;
		*(unsigned long*)(unsigned long)0x8028 =(unsigned long)idtr;
		*(unsigned long*)(unsigned long)0x8030 =(unsigned long)pml4e;
		*(unsigned long*)(unsigned long)0x8038 =(unsigned long)&ap_startup; // test
		
		apstatck = apstatck - 0x40000; // - 256KiB
		
		// send INIT IPI
		*((volatile unsigned int*)(lapic_ptr + 0x280)) = 0; // clear APIC errors
		*((volatile unsigned int*)(lapic_ptr + 0x310)) =\
		(*((volatile unsigned int*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
		*((volatile unsigned int*)(lapic_ptr + 0x300)) =\
		(*((volatile unsigned int*)(lapic_ptr + 0x300)) & 0xfff00000) | 0x00C500;  // trigger INIT IPI
		
		do { 
			__asm__ __volatile__ ("pause" : : : "memory"); 
			
		}while(*((volatile unsigned int*)(lapic_ptr + 0x300)) & (1 << 12));	// wait for delivery
		
		*((volatile unsigned int*)(lapic_ptr + 0x310)) =\
		(*((volatile unsigned int*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
		*((volatile unsigned int*)(lapic_ptr + 0x300)) =\
		(*((volatile unsigned int*)(lapic_ptr + 0x300)) & 0xfff00000) | 0x008500;  // deassert
		
		do { 
			__asm__ __volatile__ ("pause" : : : "memory"); 
		
		}while(*((volatile unsigned int*)(lapic_ptr + 0x300)) & (1 << 12));	// wait for delivery
	
		mdelay(10); // wait 10 msec
		
		// send STARTUP IPI (twice)
		for(int j = 0; j < 2; j++) {
			*((volatile unsigned int*)(lapic_ptr + 0x280)) = 0; // clear APIC errors
			*((volatile unsigned int*)(lapic_ptr + 0x310)) =\
			(*((volatile unsigned int*)(lapic_ptr + 0x310)) & 0x00ffffff) | (i << 24); // select AP
			*((volatile unsigned int*)(lapic_ptr + 0x300)) =\
			(*((volatile unsigned int*)(lapic_ptr + 0x300)) & 0xfff0f800) | 0x000608;  // trigger STARTUP IPI for 0800:0000
		
			udelay(200); // wait 200 usec
			do { 
				__asm__ __volatile__ ("pause" : : : "memory"); 
			
			}while(*((volatile unsigned int*)(lapic_ptr + 0x300)) & (1 << 12)); // wait for delivery
		}
	
	}

	
	
	printf("\naprunning %x\n", *aprunning);
}


void done()
{
	volatile unsigned int *bspdone = (volatile unsigned int *)0x8010;
	*bspdone = 1;
	
	//for(;;);
}

void ap_startup(int apicid) {

	int id = 0;
	
	do{
	__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
	: "=b"(id) );
	
	switch(id)
	{
		case 1:
			mdelay(1000);
			printf("\nLocal APIC AP1: ID = %x  %x",
			*(unsigned int *)(lapicbase + 0x20),*(unsigned int *)(lapicbase + 0x320));
			for(;;);
			break;
		
		case 2:
			break;
		
		case 3:
			break;
	
	
	}
	
	}while(1);
}










