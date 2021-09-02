#include <idt.h>
#include <stdio.h>


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

void exceptions_install(void)
{
	
    	trap(0x00,(unsigned int)isr00, 8,0);
	trap(0x01,(unsigned int)isr01, 8,0);
    	interrupter(0x02,(unsigned int)isr02, 8,0);
	trap(0x03,(unsigned int)isr03, 8,0);
	trap(0x04,(unsigned int)isr04, 8,0);
	trap(0x05,(unsigned int)isr05, 8,0);
	trap(0x06,(unsigned int)isr06, 8,0);
	trap(0x07,(unsigned int)isr07, 8,0);
	trap(0x08,(unsigned int)isr08, 8,0);
	trap(0x09,(unsigned int)isr09, 8,0);
	trap(0x0A,(unsigned int)isr10, 8,0);
	trap(0x0B,(unsigned int)isr11, 8,0);
	trap(0x0C,(unsigned int)isr12, 8,0);
	trap(0x0D,(unsigned int)isr13, 8,0);
	trap(0x0E,(unsigned int)isr14, 8,0);
	trap(0x0F,(unsigned int)isr15, 8,0);
	trap(0x10,(unsigned int)isr16, 8,0);
	trap(0x11,(unsigned int)isr17, 8,0);
	trap(0x12,(unsigned int)isr18, 8,0);
	trap(0x13,(unsigned int)isr19, 8,0);
	trap(0x24,(unsigned int)isr20, 8,0);
	trap(0x15,(unsigned int)isr21, 8,0);
	trap(0x16,(unsigned int)isr22, 8,0);
	trap(0x17,(unsigned int)isr23, 8,0);
	trap(0x18,(unsigned int)isr24, 8,0);
	trap(0x19,(unsigned int)isr25, 8,0);
	trap(0x1A,(unsigned int)isr26, 8,0);
	trap(0x1B,(unsigned int)isr27, 8,0);
	trap(0x1C,(unsigned int)isr28, 8,0);
	trap(0x1D,(unsigned int)isr29, 8,0);
	trap(0x1E,(unsigned int)isr30, 8,0);
	trap(0x1F,(unsigned int)isr31, 8,0);
	
	
	
	
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

	unsigned int cr2,cr3,cr4;
	puts(exception_mensagem[n]);
	

	if(n == 14)
	{

		__asm__ __volatile__("movl %%cr2,%k0":"=a"(cr2):);
		__asm__ __volatile__("movl %%cr3,%k0":"=a"(cr3):);
		__asm__ __volatile__("movl %%cr4,%k0":"=a"(cr4):);

		printf("CR2 = 0x%x, CR3 = 0x%x, CR4 = 0x%x\n",cr2,cr3,cr4);

	}
	
	for( ; ; );

}
