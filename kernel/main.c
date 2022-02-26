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
#include <inet.h>

#define false 0
#define true 1

int launcher;
unsigned char mult_task;
extern unsigned long server_id[5];
extern unsigned long thread_id;
extern void console();
extern void compose();
extern void window_server();
extern void setup_smp();

extern void done();
extern unsigned long gid;
extern int tmpnam;

int no;
user_t *user;
char *syspwd;

extern int setup_hda(void);

extern int play_speaker(unsigned short frequence);

static void kernel_thread(){
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    sera.sin_family        = AF_LOCAL;
    sera.sin_addr.s_addr = inet_addr("127.0.0.1");
    sera.sin_port  = htons(3000);

    serb.sin_family        = AF_LOCAL;
    serb.sin_addr.s_addr = htonl(INADDR_ANY);
    serb.sin_port  = htons(0);

    if(sd < 0) {
        printf("Cannot create socket!\n");
        while(1){}
    }
    int r = bind( sd, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        while(1){}
    }

    char b[1024];
    char *message = "sirius operating system\n\0";
    while(1){
        socklen_t len;
        ssize_t to = recvfrom(sd, b, 1024, 0, (struct sockaddr*)&serb, &len);
        ssize_t count = sendto(sd, message, strlen(message), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));
    }


}

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
    printf("Setup RAM ...\\\\\n");
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
    //ioapic_umasked(0);
	ioapic_umasked(1);
	ioapic_umasked(2);
	//ioapic_umasked(8);
    ioapic_umasked(9);
	ioapic_umasked(12);
    ioapic_umasked(11);
    ioapic_umasked(19);

    printf("Setup Graphic Adapter ...\\\\\n"); 
	initialize_graphic_adapter();
	
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

    clears_creen();
    printf("Ethernet ...\\\\\n");
    int_ethernet_device();

	/* Testing
    
	clears_creen();
	//setup_hda();
     for(;;) __asm__ __volatile__("hlt"); */
	
	user = (user_t*)malloc(sizeof(user_t));
	user->mouse = (unsigned long) mouse;
	user->clock = (unsigned long) clock;

    //
    init_tcp();
    ethernet_port_setup();

    //
    if(init_socket(AF_LOCAL, SOCK_STREAM, 0) < 0){
        printf("Cannot create socket\n");
        for(;;);
    }

    // TODO initialize ethernet
    sti();
    initialise_ethernet();
	
	cli();	
	printf("Done\n");

	thread_setup();
	mult_task = 1;
	
	unsigned long stack1, stack2;
	alloc_pages(0, 8, (unsigned long *)&stack1);
    alloc_pages(0, 8, (unsigned long *)&stack2);
    setup_vmnet();

	create_thread( &compose, stack1 + 0x7FFF, (unsigned long)pml4e, 0, 0x80,0,0, 0);
    create_thread( &window_server, stack2 + 0x7FFF, (unsigned long)pml4e, 0, 0x80,0,0, 0);
	done();

	//sti();
    printf("Wait launcher == 0\n");
	while(launcher){}
	//cli();
	
	
	printf("Initialize....\n");
    sti();
	
    unsigned long addr =0;
	alloc_pages(0, 2, (unsigned long *)&addr);
	create_thread( &kernel_thread/*&console*/, /*stack*/addr + 0x1FF0, (unsigned long)pml4e, 0, 0,0,0, 0);
	
	syspwd = (char*) malloc(0x1000);
	strcpy(syspwd,"A:");

    printf("initialize launcher.bin\n");
	FILE *fp = fopen("launcher.bin","r+b");
	if(fp) {

        fseek(fp,0,SEEK_END);
	    int e = ftell(fp);
	    rewind(fp);
        char *bf = (char*)malloc(0x40000);
        fread (bf, 1, e, fp);

		unsigned long r = exectve(0, 0, syspwd, bf);
        free(bf);
		fclose(fp);
        if(r)printf("successfull\n");
        else printf("error, elf not found\n");
	}else {
		printf("fopen error launcher.bin\n");
		//for(;;);
	}
	
	sti();
	no++;

    VERBOSE = 0;

    // TODO initialize service
	server(entry_pointer_info);
	
		
}
