#include <stdio.h>


int ungetc(int c, FILE *fp)
{
	if(!fp) return -1;

	if(c == EOF) {
		return -1;
	}
	
	if(fp->off > 0) fp->off --;
	
	fp->level = fp->off%fp->bsize;
	
	
	fp->curp = (unsigned char*)(fp->buffer + fp->level);
	*(unsigned char*)(fp->curp) = c &0xff;

	return c;
}
