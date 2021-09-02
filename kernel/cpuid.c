#include <cpuid.h>

int cpuid_vendor(char *string)
{
	unsigned int *str_p = (unsigned int*) string;
	
	__asm__ __volatile__("cpuid;":"=b"(str_p[0]),"=c"(str_p[2]),"=d"(str_p[1]):"a"(CPUID_GET_VENDOR_STRING));
	
	*(char*)((char*)str_p + 12) 	= 0;
	
	return 0;
}


int cpuid_processor_brand(char *string)
{
	unsigned int *str_p = (unsigned int*) string;
	__asm__ __volatile__("cpuid;":\
	"=a"(str_p[0]),"=b"(str_p[1]),"=c"(str_p[2]),"=d"(str_p[3]):\
	"a"(CPUID_GET_BRAND_STRING));
	
	__asm__ __volatile__("cpuid;":\
	"=a"(str_p[4]),"=b"(str_p[5]),"=c"(str_p[6]),"=d"(str_p[7]):\
	"a"(CPUID_GET_BRAND_STRING_MORE));
	
	__asm__ __volatile__("cpuid;":\
	"=a"(str_p[8]),"=b"(str_p[9]),"=c"(str_p[10]),"=d"(str_p[11]):\
	"a"(CPUID_GET_BRAND_STRING_END));
	
	
	return 0;
}

void cpuid(unsigned int code, unsigned int *a, unsigned int *d) 
{
  	__asm__ __volatile__("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}

int cpuid_string(unsigned int code, unsigned int where[4]) 
{
  	__asm__ __volatile__("cpuid":"=a"(*where),"=b"(*(where+1)),
               "=c"(*(where+2)),"=d"(*(where+3)):"a"(code));
               
  	return (int)where[0];
}







