#include <string.h>

int strcmp (const char* s1, const char* s2)
{

	const char *_s1 = s1;
	const char *_s2 = s2;

    	while((*_s1++ == *_s2++))
    		if((*_s1 == *_s2) && (*_s1 + *_s2 == 0))
    			return 0;

    	return -1;
}
