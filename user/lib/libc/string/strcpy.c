#include <string.h>

char *strcpy(char *dest, const char *src)
{
    	char *p = (char*)dest;

	char *p_dest = (char*)dest;
	char *p_src  = (char*)src;

 
    	while (*p_src != '\0') *p_dest++ = *p_src++;
    
	
	*p_dest = '\0';

	return p;
}
