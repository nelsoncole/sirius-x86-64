#include <stdio.h>


FILE *freopen(const char *filename, const char *mode, FILE *fp)
{
	printf("panic: freopen()\n");
	for(;;);
	return fp;
}
