#include <idt.h>
#include <lvt.h>
#include <irq.h>
#include <string.h>
#include <stdio.h>

extern void idt_flush(idtr_t*);

extern void syscall_install();

extern void intr_install();

static void idt_null_install();

idt_t idt[256];
idtr_t idtr[1];

static
void set_gate_idt(int n, unsigned long offset, unsigned short sel,
unsigned char type, unsigned char dpl, unsigned char p, int ist)
{

            idt[n].offset_15_0 = offset &0xFFFF;
            idt[n].sel = sel;
            idt[n].ist = ist;
            idt[n].unused = 0;
            idt[n].type = type;
            idt[n].dpl = dpl;
            idt[n].p = p;
            idt[n].offset_31_16 = offset >> 16 &0xFFFF;
            idt[n].offset_63_32 = offset >> 32 &0xFFFFFFFF;
            idt[n].reserved = 0;



}


void idt_install(void)
{

    memset(idt,0,sizeof(idt_t)*256);

    // 0 - 0xFF
    idt_null_install();
    
	// 0 - 0x1F
    exceptions_install();
    	
    // 0x20
    lvt_install();
    	
	// 0x40 -
    irq_install();
    	
    // 0x72
    syscall_install();

    // 
    intr_install();

	idtr->limit = (sizeof(idt_t)*256)-1;
    idtr->base = (unsigned long)&idt;
    
    idt_flush(idtr);
	

}

// Interrupt/Trape Gate
void idt_gate(int n, unsigned int offset,unsigned short sel, unsigned char dpl )
{
    	set_gate_idt(n,offset,sel,0x8E,dpl,1,0);

}

void _idt_gate(int n, unsigned int offset,unsigned short sel, unsigned char dpl, int ist )
{
    	set_gate_idt(n,offset,sel,0x8E,dpl,1,ist);

}


extern void intr_null();

static void idt_null_install()
{
    for(int i=0; i < 256; i++)
	    idt_gate(i,(unsigned long)intr_null, 8,0);
}


void intr_null_function(){
    printf("XXXXXXXXX\n");
}
