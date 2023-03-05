#include <acpi.h>
#include <io.h>

#include <stdio.h>
#include <string.h>

#include <delay.h>
#include <gdt.h>
#include <idt.h>
#include <paging.h>

#include <apic.h>

extern gdtr_t gdtr[1];
extern idtr_t idtr[1];

extern void screen_refresh();

typedef struct _core {
	unsigned char lapic_ids[256];	// CPU core Local APIC IDs
	unsigned char numcore;		// number of cores detected
	unsigned long lapic;		// pointer to the Local APIC MMIO registers
	unsigned long ioapic;		// pointer to the IO APIC MMIO registers
	unsigned char bspid;		// BSP id
}__attribute__ ((packed)) core_t;

core_t core[1];
void ap_startup(int apicid);

void setup_smp() {

	volatile unsigned int *bspdone = (volatile unsigned int *)(0x8000 + 8);
	volatile unsigned int *aprunning = (volatile unsigned int *)(0x8000 + 8 + 4); 
	
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
	memcpy((void*)0x8000, (void*)0x3E000, 4096); 
	unsigned long apstatck = 0x01800000 + 0x400000;
	
	*aprunning = 0;
	*bspdone = 0;

	
	for(int i = 0; i < 2/*core->numcore*/; i++) {

        if(core->numcore == 1)
            break;
		// do not start BSP, that's already running this code
		if(core->lapic_ids[i] == core->bspid) continue;
		
		*(unsigned long*)(0x8000 + 8*2) = apstatck; // stack
		*(unsigned long*)(0x8000 + 8*3) =(unsigned long)gdtr;
		*(unsigned long*)(0x8000 + 8*4) =(unsigned long)idtr;
		*(unsigned long*)(0x8000 + 8*5) =(unsigned long)pml4e;
        *(unsigned long*)(0x8000 + 8*6) =(unsigned long)apic_timer_ticks;
		*(unsigned long*)(0x8000 + 8*7) =(unsigned long)&ap_startup; // test
		
		apstatck = apstatck - 0x40000; // - 256KiB

        unsigned int apic_id = core->lapic_ids[i];

        // Send INIT IPI
        local_apic_write_command(0x280, 0); // clear APIC errors
        local_apic_send_init(apic_id);
        mdelay(100); // wait 10 msec

		/*local_apic_write_command(0x280, 0); // clear APIC errors
		local_apic_write_command(0x310, (local_apic_read_command(0x310) & 0xffffff) | (apic_id << 24) );
		local_apic_write_command(0x300, (local_apic_read_command(0x300) & 0xfff00000) | (0xC500) );
		do{
			__asm__ __volatile__("pause;":::"memory");
		}while(local_apic_read_command(0x300) & (1 << 12));
		local_apic_write_command(0x310, (local_apic_read_command(0x310) & 0xffffff) | (apic_id << 24) );
		local_apic_write_command(0x300, (local_apic_read_command(0x300) & 0xfff00000) | (0x8500) );
		do{
			__asm__ __volatile__("pause;":::"memory");
		}while(local_apic_read_command(0x300) & (1 << 12));
		mdelay(100); // wait 10 msec*/

        // Send STARTUP IPI (twice)
		for(int j=0; j < 2; j++){
			local_apic_write_command(0x280, 0); // clear APIC errors
        	local_apic_send_startup(apic_id, 0x8);
			mdelay(100);

			/*local_apic_write_command(0x280, 0); // clear APIC errors
			local_apic_write_command(0x310, (local_apic_read_command(0x310) & 0xffffff) | (apic_id << 24) );
			local_apic_write_command(0x300, (local_apic_read_command(0x300) & 0xfff0f800) | (0x608) );
			//udelay(200);
			mdelay(100);
			do{
				__asm__ __volatile__("pause;":::"memory");
			}while(local_apic_read_command(0x300) & (1 << 12));*/
		}
        
	}

	printf("\naprunning %x\n", *aprunning);

    if(*aprunning < 1) {

        printf("Panic: nenhum AP foi inicializado\n");
        for(;;)
            __asm__ __volatile__("hlt");
    }
}


void done()
{
	volatile unsigned int *bspdone = (volatile unsigned int *)(0x8000 + 8);
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
			printf("\nLocal APIC AP1: ID = %x  %x",local_apic_read_command(0x20), local_apic_read_command(0x320));
			for(;;);
			break;
		
		case 2:
			break;
		
		case 3:
			break;
	
	
	}
	
	}while(1);
}
