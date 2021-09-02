#include <stdio.h>
#include <cpuid.h>
#include <sse.h>

void fpu_cw(const short cw)
{
	__asm__ __volatile__(" fldcw %0; "::"m"(cw));
}

unsigned int sse_cr()
{
	unsigned int cr = 0x123;
	
	__asm__ __volatile__(" ldmxcsr %0; ": :"m"(cr));
	
	return cr;
}

void sse_cw( unsigned int cw)
{
	__asm__ __volatile__(" stmxcsr %0; "::"m"(cw));
}


void fpu_save(char *region)
{
	__asm__ __volatile__(" fxsave %0; " :: "m" (region));	
}


void fpu_restore(char *region)
{
	__asm__ __volatile__(" fxrstor %0; " :: "m" (region));
}


extern void EnableSSE();
extern void DisableSSE();

int setup_sse()
{
	// FIXME: inicialização improvisada do FPU 
	// CR4.OSFXSR bit 9, CR4.OSXMMEXCPT bit 10, CR4.OSXSAVE bit 18
	   
			     
	// Verificar suport x86_64
	unsigned int eax, edx;
	
	cpuid(0x1, &eax, &edx);
	
	// Test LM-bit
	if(!(edx >> 25 &1)) { 
		puts("SSE hardware not supported\n");
		for(;;);
		return 1;
	}
	
	puts("SSE is available\n");
	
	__asm__ __volatile__ (	" movq %%cr4,%%rax;  "
				" orl $0x600,%%eax;  " /*Set OSFXSR and OSXMMEXCPT*/
				" movq %%rax,%%cr4;  "
				
				" movq %%cr0,%%rax;  "
				" andw $0xFFFB,%%ax; " /*Clear EM*/
				" orw $0x2,%%ax;     " /*Set MP*/
				" movq %%rax,%%cr0;  " ::);
				
	//fpu_cw(0x37f);
	// Enable FPU
	__asm__ __volatile__ ("fninit"); 	
	
	EnableSSE();		
			  
	return 0;
}
