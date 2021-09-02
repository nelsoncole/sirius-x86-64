#include <msr.h>


int getmsr(unsigned int msr, unsigned int *low, unsigned int *hight)
{
	__asm__ __volatile__("rdmsr":"=a"(*low),"=d"(*hight):"c"(msr));
	return 0;
}

int setmsr(unsigned int msr, unsigned int low, unsigned int hight)
{
	unsigned long eax = 0, edx = 0;
	__asm__ __volatile__("rdmsr":"=a"(eax),"=d"(edx):"c"(msr));
	
	eax = eax | low;
	edx = edx | hight;
	__asm__ __volatile__("wrmsr": :"a"(eax),"d"(edx),"c"(msr));
	return 0;
}
