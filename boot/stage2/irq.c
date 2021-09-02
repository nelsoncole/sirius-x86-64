#include <idt.h>
#include <irq.h>
#include <io.h>
#include <pic.h>
#include <stdint.h>
#include <stdio.h>
#include <ps2.h>


extern void irq00();	extern void irq10();
extern void irq01();	extern void irq11();
extern void irq02();	extern void irq12();
extern void irq03();	extern void irq13();
extern void irq04();	extern void irq14();	
extern void irq05();	extern void irq15();
extern void irq06();
extern void irq07();
extern void irq08();
extern void irq09();

void irq_install(void)
{

	pic_install();
	interrupter(32,(uint32_t)irq00,0x8,0);
	interrupter(33,(uint32_t)irq01,0x8,0);
	interrupter(34,(uint32_t)irq02,0x8,0);
	interrupter(35,(uint32_t)irq03,0x8,0);
	interrupter(36,(uint32_t)irq04,0x8,0);
	interrupter(37,(uint32_t)irq05,0x8,0);
	interrupter(38,(uint32_t)irq06,0x8,0);
	interrupter(39,(uint32_t)irq07,0x8,0);
	interrupter(40,(uint32_t)irq08,0x8,0);
	interrupter(41,(uint32_t)irq09,0x8,0);
	interrupter(42,(uint32_t)irq10,0x8,0);
	interrupter(43,(uint32_t)irq11,0x8,0);
	interrupter(44,(uint32_t)irq12,0x8,0);
	interrupter(45,(uint32_t)irq13,0x8,0);
	interrupter(46,(uint32_t)irq14,0x8,0);
	interrupter(47,(uint32_t)irq15,0x8,0);

}

void default_irq(){

}

extern void ehci_handler();

void irq11_handler()
{
	ehci_handler();
	
	
}

// Lista de funções c de entrada ao manipulador de IRQs.
static void *fnvetors_handler[16] = {
    &default_irq,//&timer_handler, // 0
    &keyboard_handler, // 1
    &default_irq, // 2
    &default_irq, // 3
    &default_irq, // 4
    &default_irq, // 5
    &default_irq, // 6
    &default_irq, // 7
    &default_irq, // 8
    &default_irq, // 9
    &default_irq, // 10
    &irq11_handler, // 11
    &mouse_handler, // 12
    &default_irq, // 13
    &default_irq, //&ata_irq_handler1, // 14
    &default_irq, //&ata_irq_handler2 // 15
   
};

void irq_function(int  n)
{
	n -= 32;

    	if(n == 0 || n < 16){
    	void *addr = fnvetors_handler[n];
    	// Chamda de função correspondente
    	__asm__ __volatile__ ("call *%0"::"r"(addr));

    	}else {

        //printf("Erro irqv %d",irqv);

    	}

    	if(n >=8 )outportb(0xA0,0x20);
	outportb(0x20,0x20);

}


void irq_enable(int irq)
{
	
	unsigned short OCW1 = 0xFFFB;
	
	OCW1 &= ~(1 << irq);
	
	if(irq < 8 ) outportb(0x21, inportb(0x21) & OCW1);
	else {
	
		outportb(0xA1,inportb(0xA1) & (OCW1 >> 8));
	}

}

void irq_disable(int irq)
{

	unsigned short OCW1 = 0x0000;
	
	OCW1  |= (1 << irq);
	
	if(irq < 8 )outportb(0x21, inportb(0x21) | OCW1);
	else outportb(0xA1,inportb(0xA1) | ( OCW1 >> 8));

}
