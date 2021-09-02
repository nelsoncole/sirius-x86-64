#ifndef __GDT_H
#define __GDT_H

typedef struct _gdt{
	unsigned long long limit_15_0 :16; 
	unsigned long long base_15_0 : 16;
	unsigned long long base_23_16 : 8;
	unsigned long long type : 4;
	unsigned long long s:1;
	unsigned long long dpl: 2;
	unsigned long long p:1;
	unsigned long long limit_19_16: 4;
	unsigned long long avl :1;
	unsigned long long l:1;
	unsigned long long db:1;
	unsigned long long g:1; 
	unsigned long long base_31_24 : 8;
	 
 }__attribute__((packed)) gdt_t;
 
typedef struct _gdtr{
    	unsigned short 	limit;
	unsigned long long 	base;

}__attribute__((packed)) gdtr_t;


void gdt_install(void);
void gdt_execute_long_mode(unsigned long addr,unsigned long pointer);


#endif
