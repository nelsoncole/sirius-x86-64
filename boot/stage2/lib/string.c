#include <string.h>

void *memset(void *s, char val, size_t count)
{
	size_t _count = count;
        unsigned char *tmp = (unsigned char *)s;
        for(; _count != 0; _count--) *tmp++ = val;
        return s;
	
}

void *memcpy(void * restrict s1, const void * restrict s2, size_t n)
{	/*__asm__ __volatile__	("cld; rep movsb"
	            		: "=c"((int){0})
	            		: "D"(s1), "S"(s2), "c"(n)
	            		: "flags", "memory");
	return s1;*/

	size_t p    = n;
	char *p_dest = (char*)s1;
	char *p_src  = (char*)s2;

	while(p--)
	*p_dest++ = *p_src++;
	return s1;
}

char *strcpy(char *dest, const char *src)
{
    	char *p = (char*)dest;

	char *p_dest = (char*)dest;
	char *p_src  = (char*)src;

 
    	while (*p_src != '\0') *p_dest++ = *p_src++;
    
	
	*p_dest = '\0';

	return p;
}
size_t strlen(const char *s)
{
	char *tmp = (char*)s;
	
	while(*tmp != '\0')tmp++;

	return (size_t)(tmp - s);
}
