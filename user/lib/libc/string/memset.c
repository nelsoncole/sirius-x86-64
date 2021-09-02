#include <string.h>

void* memset(void *s, char val, size_t count)
{
	/*__asm__ __volatile__	("cld; rep stosb"
	             		: "=c"((int){0})
	             		: "D"(s), "a"(val), "c"(count)
	             		: "flags", "memory");
	return s;*/

	size_t _count = count;
        unsigned char *tmp = (unsigned char *)s;
        for(; _count != 0; _count--) *tmp++ = val;
        return s;	
}
