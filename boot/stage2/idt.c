#include <idt.h>
#include <string.h>

extern void idt_flush(idtr_t*);


idt_t idt[256];
idtr_t idtr[1];

static
void set_gate_idt(int n, unsigned int offset, unsigned short sel,
unsigned char flags, unsigned char dpl, unsigned char p)
{

            idt[n].offset_15_0 = offset &0xFFFF;
            idt[n].sel = sel;
            idt[n].unused = 0;
            idt[n].flags = flags;
            idt[n].dpl = dpl;
            idt[n].p = p;
            idt[n].offset_31_16 = offset >> 16 &0xFFFF;



}


void idt_install(void)
{

    	memset(idt,0,sizeof(idt_t)*256);
    
	// 0 - 0x1F
    	exceptions_install();
	// 0x20 -
    	irq_install();

	idtr->limit = (sizeof(idt_t)*256)-1;
    	idtr->base = (unsigned int)idt;
    
    	idt_flush(idtr);
	

}

void trap(int n, unsigned int offset,unsigned short sel, unsigned char dpl )
{
    	set_gate_idt(n,offset,sel,0x8F,dpl,1);

}

void interrupter(int n, unsigned int offset, unsigned short sel, unsigned char dpl )
{
	set_gate_idt(n,offset,sel,0x8E,dpl,1);

}
