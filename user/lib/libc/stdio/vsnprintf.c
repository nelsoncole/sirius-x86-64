#include <stdio.h>
#include <string.h>


#define BUFFER_SIZE	65536

static char sn_buf[BUFFER_SIZE];

int vsnprintf(char *s, size_t n,const char *fmt, va_list ap)
{
	int rv;
	int bytes;

	if(n > BUFFER_SIZE ) return 0;

	rv = vsprintf(sn_buf, fmt, ap);

	if(rv >= BUFFER_SIZE ) return BUFFER_SIZE; // "snprintf buffer overflow"


	bytes = rv;

	if (bytes > 0) {

		memcpy(s, sn_buf, bytes);
		s[bytes] = '\0';
    	}

	
    	return (rv);

}
