#ifndef __STDLIB_H
#define __STDLIB_H
#include <stddef.h>
#include <limits.h>


void *malloc(unsigned size);
void free(void *ptr);

void i2hex(unsigned long long val, char* dest, int len);
char *utoa(unsigned long long val, char *dst, int radix);
char *itoa (int val, char *str);





#endif
