#include <limits.h>
#include <stdlib.h>

char *itoa (int val, char *str) {
  	

	char* valuestring = (char*) str;
	int value = val;

	int min_flag;
  	char swap, *p;
 	min_flag = 0;

  	if (0 > value)
  	{
    		*valuestring++ = '-';
    		value = -INT_MAX > value ? min_flag = INT_MAX : -value;
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
