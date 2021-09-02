#ifndef __IDT_H
#define __IDT_H

typedef struct _idt{

    	unsigned long long offset_15_0 :16; 
	unsigned long long sel : 16;
	unsigned long long unused :8;
	unsigned long long flags : 5;
	unsigned long long dpl : 2;
    	unsigned long long p :1;
	unsigned long long offset_31_16 : 16;


}__attribute__ ((packed)) idt_t;

typedef struct _idtr{
    	unsigned short		limit;
	unsigned long long	base;

}__attribute__((packed)) idtr_t;



void trap(int n, unsigned int offset,unsigned short sel, unsigned char dpl );
void interrupter(int n, unsigned int offset, unsigned short sel, unsigned char dpl );

void exceptions_install(void);
void irq_install(void);

void idt_install(void);


#endif
