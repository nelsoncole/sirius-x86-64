
#include <alloc.h>
#include <stdio.h>

void *__malloc_r(unsigned size)
{
	if(!size) return (void*)0;

	unsigned long r;
	__asm__ __volatile__("int $0x72":"=a"(r):"d"(32),"D"(size));
	return (void*)r;
}

void __free_r(void *p)
{
	if(!p)return;
	__asm__ __volatile__("int $0x72": :"d"(33),"D"(p));
}


void *__realloc_r(void *ptr, unsigned size)
{
    printf("realloc: %lx %d\n", ptr, size);
	for(;;);
	unsigned long r = 0;	
	return (void*)r;
}
