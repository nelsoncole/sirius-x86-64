#include <stdio.h>



int feof (FILE *fp)
{
	if(!fp) return EOF;
	
	if(fp->off >= fp->fsize) return EOF;

	return 0;
}
