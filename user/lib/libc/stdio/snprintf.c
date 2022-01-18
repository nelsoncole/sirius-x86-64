#include <stdio.h>

int snprintf(char *s, size_t n,const char * fmt, ...)
{

	va_list ap;
    int rv;
    va_start(ap, fmt);
    rv = vsnprintf(s, n, fmt, ap);
    va_end(ap);

    return rv;

}
