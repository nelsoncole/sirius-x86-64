#include <stdio.h>

int fscanf(FILE * fp,const char * fmt, ...)
{
	va_list ap;
	int ret;
	va_start(ap, fmt);
	ret = vfscanf(fp, fmt, ap);
	va_end(ap);
	return ret;

}
