#include <string.h>

char *strchr(const char *s, int c)
{
	const char *p_s = s;
	
	for ( ;*p_s != '\0' ; ) {

		if(*p_s == c) return ((char*)p_s);

		p_s++;

	}
	
	return (NULL);
}
