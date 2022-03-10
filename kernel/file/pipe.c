#include <stdlib.h>
#include <string.h>

#include <file.h>
#include <io.h>

#define PIPE_SIZE 16

FILE *pipe_open_file(const char *filename, const char *mode)
{
	
	FILE *fp = (FILE *)malloc(sizeof(FILE));
	memset(fp,0,sizeof(FILE));
	
	fp->bsize 	= (unsigned) BUFSIZ;
	fp->buffer 	= (unsigned char*) malloc(BUFSIZ);
	fp->fsize	= 0;
	fp->mode 	= 0x2;
	fp->flags	= 0x1;
	
	memset(fp->buffer, 0, BUFSIZ);
	

	return (fp);
}

int pipe_write ( void *buf, FILE *fp)
{

    if( !fp )return 0;

	int i=0;
	unsigned char *us = (unsigned char*) buf;
    if(fp->level >= fp->bsize || fp->off >= fp->bsize) {
		
			fp->off = 0;
			fp->level = 0;
	}


	for(i=0; i < PIPE_SIZE; i++) {
	
		fp->curp = (unsigned char*)(fp->buffer + fp->off+i);
		*(unsigned char*)(fp->curp) = (*us++) &0xff;

	}

    // Update offset
	fp->off += PIPE_SIZE;
	fp->fsize += PIPE_SIZE;
	
	return (i);

}


int pipe_read ( void *buf, FILE *fp)
{	
    if( !fp )return 0;

	int i=0;
	unsigned char *us = (unsigned char*) buf;
	
	if(fp->off2 >= fp->bsize) {
	
		fp->off2 = 0;
	}
		
	while(fp->off <= fp->off2 && fp->off >= fp->off2) pause(); // wait
		
	for(i=0 ; i < PIPE_SIZE; i ++){
	
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2+i);
		*us++ = *(unsigned char*)(fp->curp) &0xff;
		
	}
    
    fp->off2 += PIPE_SIZE;

	return (i);
}

