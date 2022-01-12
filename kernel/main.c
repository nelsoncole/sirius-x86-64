#include <io.h>
#include <gui.h>
#include <paging.h>
#include <mm.h>
#include <driver.h>
#include <gdt.h>
#include <idt.h>

#include <apic.h>

#include <cpuid.h>

#include <acpi.h>
#include <hpet.h>
#include <pit.h>
#include <rtc.h>

#include <ps2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include <thread.h>
#include <storage.h>
#include <data.h>

#include <kernel.h>

#include <sse.h>

#include <ethernet.h>

#include <socket.h>

#define false 0
#define true 1

int launcher;
unsigned char mult_task;
extern unsigned long server_id[5];
extern unsigned long thread_id;
extern void console();
extern void compose();
extern void setup_smp();

extern void done();
extern unsigned long gid;
extern int tmpnam;
extern int VERBOSE;
int no;
user_t *user;
char *syspwd;

extern int setup_hda(void);

extern play_speaker(unsigned short frequence);

void main(unsigned long entry_pointer_info)
{

    VERBOSE = 1;
	launcher = 1;
	no = mult_task = 0;
	memset(server_id, 0, sizeof(unsigned long)*5);
	thread_id = 0;
	
	tmpnam = 0;
	gid =0;

    pipe_launcher = (FILE*) 0;

    interrupt_status = 1;
	
	// TODO: 
	dv_uid = *(unsigned int*)(entry_pointer_info);
	dv_num = *(unsigned int*)(entry_pointer_info + 4);
	
	initialize_gui(entry_pointer_info);
	
	printf("RUN: kernel --> x86_64\n");
	
	printf("Setup LGDT ...\\\\\n");
	printf("Setup LIDT ...\\\\\n");
	gdt_install();
	idt_install();
	
	printf("Setup Paging ...\\\\\n");
	page_install(entry_pointer_info);
	printf("Setup MM ...\\\\\n");
	initialize_mm_mp();
	ram_setup(entry_pointer_info);
	alloc_pages_setup(entry_pointer_info);
	
	setup_sse();
	setup_acpi();
	setup_apic();
	setup_ioapic();
	
	printf("Setup SMP ...\\\\\n"); 
	setup_smp();
	
	printf("Setup PIT ...\\\\\n"); 
	pit_enable(true);
	printf("Setup RTC ...\\\\\n"); 
	rtc_setup();
	apic_timer_umasked();
	ioapic_umasked(1);
	ioapic_umasked(2);
	//ioapic_umasked(8);
	ioapic_umasked(12);
    ioapic_umasked(11);
    ioapic_umasked(19);

    printf("Setup I965 ...\\\\\n"); 
	setup_i965();
	
	char cpu_name[128];
	memset(cpu_name,0,128);
	cpuid_processor_brand(cpu_name);
	
	printf("Sirius OS (Kernel mode: AMD64 or x86_64)\nCPU: %s\n",cpu_name);
	
	_stdin = stdin = fopen (0, "stdin");	
	_stdout = stdout = fopen (0, "stdout");
	_stderr = stderr = fopen (0, "stderr");
	
	// TODO drivers de interface
	printf("Setup drivers de interface ...\\\\\n");
	printf("Keyboard ...\\\\\n");  
	keyboard_install();
	printf("Mouse...\\\\\n"); 
	mouse_install();
	
	printf("ATA ...\\\\\n"); 
	ata_initialize();

    printf("Ethernet ...\\\\\n");
    int_ethernet_device();
    
    // TODO initialize ethernet
    sti();
    initialise_ethernet();
    cli();

	/* Testing
    
	clears_creen();
	//setup_hda();
    for(;;); */
	
	user = (user_t*)malloc(sizeof(user_t));
	user->mouse = (unsigned long) mouse;
	user->clock = (unsigned long) clock;

    //
    if(init_socket(AF_LOCAL, SOCK_STREAM, 0) < 0){
        printf("Cannot create socket\n");
    }
	
	cli();	
	printf("Done\n");

	thread_setup();
	mult_task = 1;
	
	unsigned long stack;
	alloc_pages(1, 8, (unsigned long *)&stack);
	
	create_thread( &compose, stack + 0x7FFF, (unsigned long)pml4e, 0, 0x80,0,0, 0);
	done();
	
	sti();

	while(launcher)
		__asm__ __volatile__("pause" :::"memory");
	cli();
	
	
	printf("Initialize....\n");
	
	//alloc_pages(1, 8, (unsigned long *)&stack);
	//create_thread( &console, stack + 0x7FFF, (unsigned long)pml4e, 0, 0,0,0, 0);
	
	syspwd = (char*) malloc(0x1000);
	strcpy(syspwd,"A:");

	FILE *fp = fopen("launcher.bin","r+b");
	if(fp) {
		exectve(0, 0, syspwd, fp);
		fclose(fp);
	}else {
		printf("fopen error launcher.bin\n");
		//for(;;);
	}
	
	
	sti();
	// HPET bug no vitual box
	//sleep(1000);
	
	no++;
	/*
	unsigned long entry, rsp;
	alloc_pages(1, 1, (unsigned long *)&entry);
	alloc_pages(1, 2, (unsigned long *)&rsp);
	
	FILE *fz = fopen("a.bin", "r+b");
	fread((void*)entry, 1, 0x1000,fz);
	fclose(fz);
	rsp += 0x1000;
	
	__asm__ __volatile__ (
      	" movq $0, %%rax;    	\n"
        " mov %%ax, %%ds;   	\n"
        " mov %%ax, %%es;   	\n"
        " mov %%ax, %%fs;  		\n"
        " mov %%ax, %%gs; 		\n"
        " movq %0, %%rax;		\n"
        " movq %1, %%rcx;		\n"
        " movq %1, %%rsp;		\n"
        " movq $0, %%rbp;		\n"
        " pushq $0x23;        	\n"
        " pushq %%rcx;      	\n"
        " pushq $0x3002;		\n"
		" pushq $0x1B;     		\n"
		" pushq %%rax;     		\n"
		" iretq;				\n"::"D"(entry), "S"(rsp));*/
		
	//play_speaker(0x10000);


    // TODO initialize service
	server(entry_pointer_info);
	
		
}
