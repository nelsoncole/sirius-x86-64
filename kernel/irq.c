#include <idt.h>
#include <irq.h>
#include <apic.h>

#include <stdio.h>

#include <data.h>

#include <io.h>

extern void irq0();	extern void irq10();	extern void irq20();
extern void irq1();	extern void irq11();	extern void irq21();
extern void irq2();	extern void irq12();	extern void irq22();
extern void irq3();	extern void irq13();	extern void irq23();
extern void irq4();	extern void irq14();	
extern void irq5();	extern void irq15();
extern void irq6();	extern void irq16();
extern void irq7();	extern void irq17();
extern void irq8();	extern void irq18();
extern void irq9();	extern void irq19();


void default_irq(int n){
	
	printf("Null IRQ n: %d\n",n); 
}

// Lista de funções c de entrada ao manipulador de IRQs.
void *fnvetors_handler[24] = {
    &default_irq, // 0
    &default_irq, // 1
    &default_irq, // 2
    &default_irq, // 3
    &default_irq, // 4
    &default_irq, // 5
    &default_irq, // 6
    &default_irq, // 7
    &default_irq, // 8
    &default_irq, // 9
    &default_irq, // 10
    &default_irq, // 11
    &default_irq, // 12
    &default_irq, // 13
    &default_irq, // 14
    &default_irq, // 15
    &default_irq, // 16
    &default_irq, // 17
    &default_irq, // 18
    &default_irq, // 19
    &default_irq, // 20
    &default_irq, // 21
    &default_irq, // 22
    &default_irq, // 23
};

extern void call_function(void *, int);
void irq_function(unsigned int n){

	n -= 0x40;
	
	apic_eoi_register();

    if(n < 24){
    	
    	void *addr = fnvetors_handler[n];
    	// Chamda de função correspondente
    	call_function(addr, n);

    }else printf("Erro IRQ%d\n", n);

    if(n == 1 || n == 12){
        interrupt_status = 1;
    }

    /*
    outportb(0xA0,0x20);
	outportb(0x20,0x20); */
	
	
}


void irq_install()
{
	idt_gate(0x40,(unsigned long)irq0, 8,0);
	idt_gate(0x41,(unsigned long)irq1, 8,0);
	idt_gate(0x42,(unsigned long)irq2, 8,0);
	idt_gate(0x43,(unsigned long)irq3, 8,0);
	idt_gate(0x44,(unsigned long)irq4, 8,0);
	idt_gate(0x45,(unsigned long)irq5, 8,0);
	idt_gate(0x46,(unsigned long)irq6, 8,0);
	idt_gate(0x47,(unsigned long)irq7, 8,0);
	idt_gate(0x48,(unsigned long)irq8, 8,0);
	idt_gate(0x49,(unsigned long)irq9, 8,0);
	idt_gate(0x4A,(unsigned long)irq10, 8,0);
	idt_gate(0x4B,(unsigned long)irq11, 8,0);
	idt_gate(0x4C,(unsigned long)irq12, 8,0);
	idt_gate(0x4D,(unsigned long)irq13, 8,0);
	idt_gate(0x4E,(unsigned long)irq14, 8,0);
	idt_gate(0x4F,(unsigned long)irq15, 8,0);
	idt_gate(0x50,(unsigned long)irq16, 8,0);
	idt_gate(0x51,(unsigned long)irq17, 8,0);
	idt_gate(0x52,(unsigned long)irq18, 8,0);
	idt_gate(0x53,(unsigned long)irq19, 8,0);
	idt_gate(0x54,(unsigned long)irq20, 8,0);
	idt_gate(0x55,(unsigned long)irq21, 8,0);
	idt_gate(0x56,(unsigned long)irq22, 8,0);
	idt_gate(0x57,(unsigned long)irq23, 8,0);
}


