#include <stdio.h>


int sprintf(char * str, const char * fmt, ...)
{
	int ret;
  	va_list ap;
  	va_start (ap, fmt);
  	ret = vsprintf(str, fmt, ap);
  	va_end (ap);

	*(unsigned char*)(str + ret) = '\0';	/* terminate the string */
	
  	return ret;

}
