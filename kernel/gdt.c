#include <gdt.h>
#include <tss.h>
#include <string.h>

extern void gdt_flush(gdtr_t*);

gdt_t gdt[32];
gdtr_t gdtr[1];

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


void gdt_install()
{

	
	
	memset(gdt,0,sizeof(gdt_t)*32);
	//     (gdt,n,limit ,base ,type,s,dpl,p,avl,l,db,g)
	set_gdt(gdt,0,0,0,0,0,0,0,0,0,0,0);
	set_gdt(gdt,1,0,0x0,0xA,1,0,1,0,1,0,0); //dpl 0
	set_gdt(gdt,2,0,0x0,0x2,1,0,1,0,1,0,0); //dpl 0
	
	set_gdt(gdt,3,0,0x0,0xA,1,3,1,0,1,0,0); //dpl 3
	set_gdt(gdt,4,0,0x0,0x2,1,3,1,0,1,0,0); //dpl 3
	
	set_gdt(gdt,5,sizeof(TSS)-1,(unsigned long)tss,0x9,0,3,1,0,0,0,1); //tss dpl 3
	set_gdt(gdt,6,(unsigned long)tss >> 32, (unsigned long)tss >> 48,0,0,0,0,0,0,0,0);


    gdtr->limit 	= (sizeof(gdt_t)*32)-1;
    gdtr->base 	= (unsigned long)gdt;
        
    gdt_flush(gdtr);
        
    tss_install();
    load_ltr(0x2b);   // RPL 3 user
     /*   
    clears_creen();
    
    
    printf("DEBUG GDT TSS:\n");
    printf("LIMIT_15_0 : 0x%x\n", gdt[5].limit_15_0);
    printf("BASE_15_0  : 0x%x\n", gdt[5].base_15_0);
    printf("BASE_23_16 : 0x%x\n", gdt[5].base_23_16);
    printf("TYPE       : 0x%x\n", gdt[5].type);
    printf("S          : 0x%x\n", gdt[5].s);
    printf("DPL        : 0x%x\n", gdt[5].dpl);
    printf("P          : 0x%x\n", gdt[5].p);
    printf("LIMIT_19_16: 0x%x\n", gdt[5].limit_19_16);
    printf("AVL        : 0x%x\n", gdt[5].avl);
    printf("L          : 0x%x\n", gdt[5].l);
    printf("DB         : 0x%x\n", gdt[5].db);
    printf("G          : 0x%x\n", gdt[5].g);
    printf("BASE_31_24 : 0x%x\n", gdt[5].base_31_24);
    
    printf("DEBUG GDT TSS >> 32:\n");
    printf("LIMIT_15_0 : 0x%x\n", gdt[6].limit_15_0);
    printf("BASE_15_0  : 0x%x\n", gdt[6].base_15_0);
    printf("BASE_23_16 : 0x%x\n", gdt[6].base_23_16);
    printf("TYPE       : 0x%x\n", gdt[6].type);
    printf("S          : 0x%x\n", gdt[6].s);
    printf("DPL        : 0x%x\n", gdt[6].dpl);
    printf("P          : 0x%x\n", gdt[6].p);
    printf("LIMIT_19_16: 0x%x\n", gdt[6].limit_19_16);
    printf("AVL        : 0x%x\n", gdt[6].avl);
    printf("L          : 0x%x\n", gdt[6].l);
    printf("DB         : 0x%x\n", gdt[6].db);
    printf("G          : 0x%x\n", gdt[6].g);
    printf("BASE_31_24 : 0x%x\n", gdt[6].base_31_24);
        
    for(;;);*/
  
}

