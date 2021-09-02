#include <string.h>

void *memcpy(void * restrict s1, const void * restrict s2, size_t n)
{	
	size_t p    = n;
	char *p_dest = (char*)s1;
	char *p_src  = (char*)s2;

	while(p--)
	*p_dest++ = *p_src++;
	return s1;
}
