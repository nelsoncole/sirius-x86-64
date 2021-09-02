#include <string.h>

char *strrchr(const char *s, int c) 
{

	size_t len = strlen(s);

	const char *p_s = s + len;
	int i;

	for ( i=0; i < len; i++ ) {

		if(*p_s == c) return ((char*)p_s);

		p_s--;
	}

	
	return (NULL);

}
