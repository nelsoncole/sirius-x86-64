#ifndef __IDT_H
#define __IDT_H

typedef struct _idt{

    	unsigned long offset_15_0 :16; 
	unsigned long sel : 16;
	unsigned long ist:3;
	unsigned long unused :5;
	unsigned long type : 5;
	unsigned long dpl : 2;
    	unsigned long p :1;
	unsigned long offset_31_16 : 16;
	unsigned long offset_63_32 : 32;
	unsigned long reserved : 32;


}__attribute__ ((packed)) idt_t;

typedef struct _idtr{
    	unsigned short	limit;
	unsigned long  base;

}__attribute__((packed)) idtr_t;



void idt_gate(int n, unsigned int offset,unsigned short sel, unsigned char dpl );
void _idt_gate(int n, unsigned int offset,unsigned short sel, unsigned char dpl, int ist );

void exceptions_install(void);
void irq_install(void);

void idt_install(void);


#endif
