#include <stdlib.h>
#include <string.h>

void initialize_heap(unsigned int start)
{
	heap = start;
	
}

void *malloc(size_t size)
{
	heap += size;
	return (void*)(heap - size);

}

void free(void *buffer)
{

}

#define HEX_LEN 8

void i2hex(unsigned int val, char* dest, int len)
{
	/*char* cp;
	char x;
	int n;
	n = val;
	cp = &dest[len];
	while (cp > dest)
	{
		x = n & 0xF;
		n >>= 4;
		*--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0');
	}
    dest[len]='\0';*/

	char* cp;
	int i, x;
	unsigned n;
	
	if(val == 0) {
		cp = &dest[0];
		*cp++ = '0';
		*cp = '\0';
		return;
	}
	

	n = val;
	cp = &dest[len];
	while (cp > dest)
	{
		x = n & 0xF;
		n >>= 4;
		*--cp = x + ((x > (HEX_LEN+1)) ? 'A' - 10 : '0');
	}
    
	dest[len]='\0';

	cp = &dest[0];
	for(i=0; i < len;i++) {
	
		if(*cp == '0') {
			cp++;
		}
		else {
			strcpy(dest,cp);
			 break;
		}
			
	}

	cp = &dest[0];
	n = strlen(cp);
	memset(dest + n,0,8-n);
}

char *itoa (int val, char *str) 
{
  	

	char* valuestring = (char*) str;
	int value = val;

	int min_flag;
  	char swap, *p;
 	min_flag = 0;

  	if (0 > value)
  	{
    		*valuestring++ = '-';
    		value = -INT_MAX> value ? min_flag = INT_MAX : -value;
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
