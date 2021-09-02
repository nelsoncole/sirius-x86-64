#ifndef __STRING_H
#define __STRING_H
#include <stddef.h>

void *memset(void *s, char val, size_t count);
void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);


#endif
