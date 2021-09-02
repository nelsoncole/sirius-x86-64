#ifndef __ALLOC_H
#define __ALLOC_H


#define REALLOC_FIRST_SIZE 131072*8 // 128KiB

#define ALLOC_COUNT 256

extern unsigned long __m_i;

typedef struct __alloc
{
	unsigned long addr;
	unsigned long sub;
	unsigned int size;
	unsigned int pool;
	unsigned int flag;
	
}__attribute__ ((packed)) __alloc_t;


#endif
