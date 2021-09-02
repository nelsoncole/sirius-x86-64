#include <stdio.h>

#define pause() __asm__ __volatile__ ("pause":::"memory")

#define PIPE_SIZE 16

int pipe_write ( void *buf, FILE *fp)
{

    if( !fp )return 0;

	int i=0;
	unsigned char *us = (unsigned char*) buf;

	for( ; i < PIPE_SIZE; i ++) {
	
		if(fp->level >= fp->bsize || fp->off >= fp->bsize) {
		
			fp->off = 0;
			fp->level = 0;
		}
	
		fp->curp = (unsigned char*)(fp->buffer + fp->off);
		*(unsigned char*)(fp->curp) = *us ++ &0xff;

		// Update offset
		fp->off += 1;
	
		if(fp->off > fp->fsize ) fp->fsize += 1;
	}
	
	return (i);

}


int pipe_read ( void *buf, FILE *fp)
{	
    if( !fp )return 0;

	int i=0;
	unsigned char *us = (unsigned char*) buf;
	
	fp->off2 += PIPE_SIZE;
	
	if(fp->off2 >= fp->bsize) {
	
		fp->off2 = PIPE_SIZE;
	}
		
	while(fp->off < fp->off2) pause(); // wait
	
	fp->off2 -= PIPE_SIZE;
		
	for( ; i < PIPE_SIZE; i ++){
	
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		*us ++ = *(unsigned char*)(fp->curp) &0xff;
		
		fp->off2 += 1;
		
	}
	return (i);
}

int pipe_read_2x ( void *buf, FILE *fp)
{
    if( !fp )return 0;
	
	int i=0;
	unsigned char *us = (unsigned char*) buf;
	
	fp->off2 += PIPE_SIZE;
	
	if(fp->off2 >= fp->bsize) {
	
		fp->off2 = PIPE_SIZE;
	}
	
    if( fp->off < fp->off2 ) {
        fp->off2 -= PIPE_SIZE;
        return 0;
    }

	fp->off2 -= PIPE_SIZE;
		
	for( ; i < PIPE_SIZE; i ++){
	
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		*us ++ = *(unsigned char*)(fp->curp) &0xff;
		
		fp->off2 += 1;
		
	}
	return (i);
}


