#ifndef __STRING_H
#define __STRING_H
#include <stddef.h>

void *memset(void *s, char val, size_t count);
void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int memcmp(char *s1, char *s2, int n);

int strcmp (const char* s1, const char* s2);


#endif
