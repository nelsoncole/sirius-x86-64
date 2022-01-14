#include <idt.h>
#include <irq.h>
#include <apic.h>

#include <stdio.h>

extern void intr255();

void intr_install()
{
	idt_gate(0xff,(unsigned long)intr255, 8,0);
}


void intr_function(){
    printf("Spurious\n");
    apic_eoi_register();
}
