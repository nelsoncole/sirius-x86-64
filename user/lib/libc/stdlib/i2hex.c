#include <stdlib.h>
#include <string.h>

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
