#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


char *ltoa_r (long long val, char *str) {
  	

	char* valuestring = (char*) str;
	long long value = val;

	long long min_flag;
  	char swap, *p;
 	min_flag = 0;

  	if (0 > value)
  	{
    		*valuestring++ = '-';
    		value = -LLONG_MAX> value ? min_flag = LLONG_MAX : -value;
  	}

  	p = valuestring;

  	do
  	{
    		*p++ = (char)(value % 10) + '0';
    		value /= 10;
  	} while (value);

  	if (min_flag != 0)
  	{
    		++*valuestring;
  	}
  	*p-- = '\0';

  	while (p > valuestring)
  	{
    		swap = *valuestring;
    		*valuestring++ = *p;
    		*p-- = swap;
  	}

	return str;
}

char *ltoa(long long val, char *dst, int radix)
{
        char* cp = (char*) dst;

	if(val < 0) {
         *cp++ = '-';
         val = -val;
    	}

   	long long l = (long long)val;
	if(radix == 16)
		i2hex(l, cp, 16);
	else 
		ltoa_r (l, cp);
		
        return dst;
}
