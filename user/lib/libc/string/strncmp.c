#include <string.h>

int strncmp (const char* s1,const char* s2,size_t count)
{

	const char *_s1 = s1;
	const char *_s2 = s2;

	
	if(!count)
	return -1;

	while(--count)
	if(*_s1++ != *_s2++)
	break;

	return (count);
}
