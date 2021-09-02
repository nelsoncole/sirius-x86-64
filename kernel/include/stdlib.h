#ifndef __STDLIB_H
#define __STDLIB_H
#include <stddef.h>
#include <limits.h>


void *malloc(unsigned size);
void free(void *ptr);

void i2hex(unsigned int val, char* dest, int len);
char *itoa (int val, char *str);





#endif
