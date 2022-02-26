#include <idt.h>
#include <stdio.h>
#include <mm.h>
#include <thread.h>


extern void isr00();	extern void isr10();	extern void isr20();	extern void isr30();		
extern void isr01();	extern void isr11();	extern void isr21();	extern void isr31();
extern void isr02();	extern void isr12();	extern void isr22();
extern void isr03();	extern void isr13();	extern void isr23();
extern void isr04();	extern void isr14();	extern void isr24();
extern void isr05();	extern void isr15();	extern void isr25();
extern void isr06();	extern void isr16();	extern void isr26();
extern void isr07();	extern void isr17();	extern void isr27();
extern void isr08();	extern void isr18();	extern void isr28();
extern void isr09();	extern void isr19();	extern void isr29();

extern int screan_spin_lock;
void exceptions_install(void)
{
	
    idt_gate(0x00,(unsigned long)isr00, 8,0);
	idt_gate(0x01,(unsigned long)isr01, 8,0);
    idt_gate(0x02,(unsigned long)isr02, 8,0);
	idt_gate(0x03,(unsigned long)isr03, 8,0);
	idt_gate(0x04,(unsigned long)isr04, 8,0);
	idt_gate(0x05,(unsigned long)isr05, 8,0);
	idt_gate(0x06,(unsigned long)isr06, 8,0);
	idt_gate(0x07,(unsigned long)isr07, 8,0);
	idt_gate(0x08,(unsigned long)isr08, 8,0);
	idt_gate(0x09,(unsigned long)isr09, 8,0);
	idt_gate(0x0A,(unsigned long)isr10, 8,0);
	idt_gate(0x0B,(unsigned long)isr11, 8,0);
	idt_gate(0x0C,(unsigned long)isr12, 8,0);
	idt_gate(0x0D,(unsigned long)isr13, 8,0);
	idt_gate(0x0E,(unsigned long)isr14, 8,0);
	idt_gate(0x0F,(unsigned long)isr15, 8,0);
	idt_gate(0x10,(unsigned long)isr16, 8,0);
	idt_gate(0x11,(unsigned long)isr17, 8,0);
	idt_gate(0x12,(unsigned long)isr18, 8,0);
	idt_gate(0x13,(unsigned long)isr19, 8,0);
	idt_gate(0x24,(unsigned long)isr20, 8,0);
	idt_gate(0x15,(unsigned long)isr21, 8,0);
	idt_gate(0x16,(unsigned long)isr22, 8,0);
	idt_gate(0x17,(unsigned long)isr23, 8,0);
	idt_gate(0x18,(unsigned long)isr24, 8,0);
	idt_gate(0x19,(unsigned long)isr25, 8,0);
	idt_gate(0x1A,(unsigned long)isr26, 8,0);
	idt_gate(0x1B,(unsigned long)isr27, 8,0);
	idt_gate(0x1C,(unsigned long)isr28, 8,0);
	idt_gate(0x1D,(unsigned long)isr29, 8,0);
	idt_gate(0x1E,(unsigned long)isr30, 8,0);
	idt_gate(0x1F,(unsigned long)isr31, 8,0);

}


char *exception_mensagem[32]={
	"Divide error\n",		// 0 - fault  -- no error 
	"Debug exception\n", 	// 1 - fault/trap -- no error 
	"NMI Interrupter\n",	//  2 - interrupter -- no error
	"Breakpoint\n",		// 3 - trap -- no error
	"Overflow\n",		// 4 - trap -- no error
	"BOUND Ranger exception\n", // 5 - trap -- no error
	"Invalide opcode (Undefined opcode)\n", // 6 - fault -- no error
	"Device not avaliable (Not Math coprocessor\n)", // 7 - fault -- no error
	"Double Fault (Erro de codigo)\n",		// 8 - about -- error code
	"Coprocessor segment overrun (reservado)\n",	// 9 - fault -- no error
	"Invalide TSS (Erro de codigo)\n", 	// 10 - fault -- error code
	"Segment not present (Erro de codigo)\n",		// 11 - fault -- error code
	"Stack segment fault (Erro de codigo)\n",	// 12 - fault -- error code
	"General protetion (Erro de codigo)\n",	// 13 - fault -- error code
	"Page fault (Erro de codigo)\n",		// 14 - fault -- error code
	"Intel reserved do not use 15\n",		// 15
	"x87 FPU Floating-Point error (Math fault)\n",	// 16 - fault -- no error
	"Alignment check (Erro de codigo)\n",		// 17 - fault -- error code
	"Machine check\n",	// 18 - about -- no error 
	"SIND Floating-Point exception\n", // 19 - fault -- no error
	"Virtualization exception\n",	// 20 - fault -- no error
	"Intel reserved do not use 21\n",	// 21
	"Intel reserved do not use 22\n",	// 22
	"Intel reserved do not use 23\n",	// 23
	"Intel reserved do not use 24\n",	// 24
	"Intel reserved do not use 25\n",	// 25
	"Intel reserved do not use 26\n",	// 26
	"Intel reserved do not use 27\n",	// 27
	"Intel reserved do not use 28\n",	// 28
	"Intel reserved do not use 29\n",    // 29
	"Intel reserved do not use 30\n",	// 30
	"Intel reserved do not use 31\n",	// 31

};



void fault_exception(int  n)
{
    VERBOSE = 1;

	while(screan_spin_lock);
	screan_spin_lock++;

	unsigned long cr2, cr3, cr4;
	puts(exception_mensagem[n]);

    int id = 0;
	__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
	: "=b"(id) );
	

	if(n == 14)
	{

		__asm__ __volatile__("movq %%cr2,%0":"=a"(cr2):);
		__asm__ __volatile__("movq %%cr3,%0":"=a"(cr3):);
		__asm__ __volatile__("movq %%cr4,%0":"=a"(cr4):);
		
		unsigned long p = get_phy_addr (cr2);

		printf("CR2 = 0x%x, CR3 = 0x%x, CR4 = 0x%x\n",cr2,cr3,cr4);
		printf("PYSICAL: 0x%x\n", p);

	}
	
    printf("Local APIC ID: %x\n",id);
    THREAD	*tmp = current_thread1;
    if(id > 0){
        tmp = current_thread2;
    }
	printf("PID:%d\n",tmp->pid);
	
	for( ; ; );
	screan_spin_lock = 0;
}
