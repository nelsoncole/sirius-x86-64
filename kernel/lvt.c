#include <idt.h>
#include <lvt.h>
#include <apic.h>

#include <stdio.h>


extern void lvt0(void);
extern void lvt1(void);
extern void lvt2(void);
extern void lvt3(void);
extern void lvt4(void);

extern void timer1(void);
extern void timer2(void);
extern void timer3(void);
extern void timer4(void);
extern void timer5(void);
extern void timer6(void);
extern void timer7(void);
extern void timer8(void);

void lvt_function(int n)
{

	apic_eoi_register();
	
	switch(n - 0x20){
		
		case 0: // APIC Timer
			apic_initial_count_timer(1000);
			//printf("APIC_LVT_TIMER\n");
		break;
		case 1: // APIC_LVT_PERFORMANCE
		
			printf("APIC_LVT_PERFORMANCE\n");
		break;
		case 2: // APIC_LVT_LINT0
			printf("APIC_LVT_LINT0\n");
		break;
		case 3: // APIC_LVT_LINT1
			printf("APIC_LVT_LINT1\n");
		break;
		case 4: // APIC_LVT_ERROR

			printf("APIC_LVT_ERROR\n");
		break;
		case 5: // APIC Timer1
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER 1\n");
		break;
		case 6: // APIC Timer2
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER 2\n");
		break;
		case 7: // APIC Timer3
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER 3\n");
		break;
		case 8: // APIC Timer4
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER\n");
		break;
		case 9: // APIC Timer5
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER\n");
		break;
		case 10: // APIC Timer6
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER\n");
		break;
		case 11: // APIC Timer7
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER\n");
		break;
		case 12: // APIC Timer8
			apic_initial_count_timer(12345);
			//printf("APIC_LVT_TIMER\n");
		break;
		default:
			printf("Default LVT n: %d\n",n - 0x20);
		break;
	}
}


void lvt_install()
{
	
	idt_gate(0x20,(unsigned long)lvt0, 8,0);
	idt_gate(0x21,(unsigned long)lvt1, 8,0);
	idt_gate(0x22,(unsigned long)lvt2, 8,0);
	idt_gate(0x23,(unsigned long)lvt3, 8,0);
	idt_gate(0x24,(unsigned long)lvt4, 8,0);
	
	idt_gate(0x25,(unsigned long)timer1, 8,0);
	idt_gate(0x26,(unsigned long)timer2, 8,0);
	idt_gate(0x27,(unsigned long)timer3, 8,0);
	idt_gate(0x28,(unsigned long)timer4, 8,0);
	idt_gate(0x29,(unsigned long)timer5, 8,0);
	idt_gate(0x2A,(unsigned long)timer6, 8,0);
	idt_gate(0x2B,(unsigned long)timer7, 8,0);
	idt_gate(0x2C,(unsigned long)timer8, 8,0);
}
