#include <stdlib.h>

extern void *__realloc_r(void *ptr, int s);

void *realloc(void *ptr, size_t size)
{
	return (void*) __realloc_r(ptr, size);
}
