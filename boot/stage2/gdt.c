#include <gdt.h>
#include <string.h>

extern void gdt_flush(gdtr_t*);

gdt_t gdt1[3], gdt2[3];
gdtr_t gdtr1[1], gdtr2[1];

static
void set_gdt(gdt_t *gdt,int n,unsigned int limit, unsigned int base, unsigned char type,
unsigned char s, unsigned char dpl, unsigned char p, unsigned char avl, unsigned char l,
unsigned char db, unsigned char g)
{


	gdt[n].limit_15_0 = limit &0xFFFF;
	gdt[n].base_15_0 = base &0xFFFF;
	gdt[n].base_23_16 = base >> 16 &0xFF;
	gdt[n].type=type &0xF;
	gdt[n].s=s &0x1;
	gdt[n].dpl =dpl &0x3;
	gdt[n].p=p &0x1;
	gdt[n].limit_19_16= limit >>16 &0xF;
	gdt[n].avl=avl &1;
	gdt[n].l=l &1;
	gdt[n].db=db &1;
	gdt[n].g=g &1;
	gdt[n].base_31_24 = base  >> 24 &0xFF;

}


void gdt_install(void)
{

	memset(gdt1,0,sizeof(gdt_t)*3);
	//     (n,limit ,base ,type,s,dpl,p,avl,l,db,g)
	set_gdt(gdt1,0,0,0,0,0,0,0,0,0,0,0);
	set_gdt(gdt1,1,0xFFFFF,0x00000000,0xA,1,0,1,0,0,1,1); //dpl 0
	set_gdt(gdt1,2,0xFFFFF,0x00000000,0x2,1,0,1,0,0,1,1); //dpl 0


    	gdtr1->limit 	= (sizeof(gdt_t)*3)-1;
    	gdtr1->base 	= (unsigned int)gdt1;
    	
        gdt_flush(gdtr1);
  
}


extern void long_mode(gdtr_t*,unsigned long addr,unsigned long bootblock);
void gdt_execute_long_mode(unsigned long addr,unsigned long bootblock)
{
	
	memset(gdt2,0,sizeof(gdt_t)*3);
	//     (n,limit ,base ,type,s,dpl,p,avl,l,db,g)
	set_gdt(gdt2,0,0,0,0,0,0,0,0,0,0,0);
	set_gdt(gdt2,1,0,0x00000000,0xA,1,0,1,0,1,0,0); //dpl 0
	set_gdt(gdt2,2,0,0x00000000,0x2,1,0,1,0,1,0,0); //dpl 0


    	gdtr2->limit 	= (sizeof(gdt_t)*3)-1;
    	gdtr2->base 	= (unsigned int)gdt2;
    	
        long_mode(gdtr2,addr,bootblock);
  
}

