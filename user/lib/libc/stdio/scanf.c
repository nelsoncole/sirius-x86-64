#include <stdio.h>

int scanf(char *fmt, ...)
{
	va_list ap;
	int ret;
	va_start(ap, fmt);
	ret = vfscanf(stdin, fmt, ap);
	va_end(ap);
	return ret;
}
