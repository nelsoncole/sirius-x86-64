#include <string.h>

int strncpy(char *dest, const char *src,size_t count)
{

	char *p_dest = (char*)dest;
	char *p_src  = (char*)src;

    	int i;
	for(i =0;i < count;i++) *p_dest++ = *p_src++;

	*p_dest ='\0';

    	return i;
}
