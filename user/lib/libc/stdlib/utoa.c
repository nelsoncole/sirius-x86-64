#include <stdlib.h>
#include <stdio.h>

#include <limits.h>


static char *utoa_r (unsigned long long val, char *str) {
  	

	char* valuestring = (char*) str;
	unsigned long long value = val;
  	char swap, *p;

  	p = valuestring;

  	do
  	{
    		*p++ = (char)(value % 10) + '0';
    		value /= 10;
  	} while (value);

  	*p-- = '\0';

  	while (p > valuestring)
  	{
    		swap = *valuestring;
    		*valuestring++ = *p;
    		*p-- = swap;
  	}

	return str;
}

char *utoa(unsigned long long val, char *dst, int radix)
{
        char* cp = (char*) dst;

        unsigned long long lu = (unsigned long long)val;

	if(radix == 16)
		i2hex(lu, cp, 16);
	else 
		utoa_r (lu, cp);
		
        return dst;

	return dst;
}
