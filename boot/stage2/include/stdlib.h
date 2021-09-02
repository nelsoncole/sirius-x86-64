#ifndef __STDLIB_H
#define __STDLIB_H
#include <stddef.h>
#include <limits.h>
#include <data.h>

void initialize_heap(unsigned int start);
void *malloc(size_t size);
void free(void *buffer);
void i2hex(unsigned int val, char* dest, int len);
char *itoa (int val, char *str);



#endif
