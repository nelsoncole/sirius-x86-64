#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static unsigned long __precision(int precision)
{
 	unsigned long mul = 1;
 	for(int i=0; i < precision; i ++) mul *= 10;
 
 	return mul;
}

char *ftoa(double f, char * buf, int precision)
{
	if(buf == 0) return buf;
	

   	char 	  *cp = buf;
    	unsigned long l, rem;


    	if(f < 0) {
        	*cp++ = '-';
        	f = -f;
    	}
    
    	l = (unsigned long)f;
    	f -= (double)l;
    	rem = (unsigned long)(f * __precision(precision));
    
    	sprintf(cp, "%lu.%lu", l, rem);
    	//sprintf(cp, "%u.%u", l, rem);
    
    	return buf;
}

