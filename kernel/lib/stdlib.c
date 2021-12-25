#include <stdlib.h>
#include <string.h>

#include <mm.h>

void *malloc(unsigned size) {

	unsigned long addr;
	
	int len = size/0x1000;
	if(size%0x1000) len++;
	
	alloc_pages(1, len, &addr);
	
	return (void*)(addr);

}


void free(void *ptr) {

	free_pages(ptr);
}

void i2hex(unsigned long long val, char* dest, int len)
{
	char* cp;
	int i, x;
	unsigned long long n;
	
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
		*--cp = x + ((x > 9) ? 'A' - 10 : '0');
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
	memset(dest + n,0,len-n);
}

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

