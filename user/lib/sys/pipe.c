#include <stdio.h>

#define pause() __asm__ __volatile__ ("pause":::"memory")

#define PIPE_SIZE 16

int pipe_write ( void *buf, FILE *fp)
{

    if( !fp )return 0;

	int i=0;
	unsigned char *us = (unsigned char*) buf;
    if(fp->level >= fp->bsize || fp->off >= fp->bsize) {
		
			fp->off = 0;
			fp->level = 0;
	}


	for(i=0; i < PIPE_SIZE; i ++) {
	
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
    //printf("debug %d %d %d\n", fp->off, fp->off2, fp->bsize);
	
	if(fp->off2 >= fp->bsize) {
	
	    fp->off2 = 0;
	}
		
	while(fp->off <= fp->off2 || ( (int)fp->off - (int)fp->off2 ) < 0) pause(); // wait
		
	for(i=0 ; i < PIPE_SIZE; i ++){
	
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2+i);
		*us++ = *(unsigned char*)(fp->curp) &0xff;
		
	}
    
    fp->off2 += PIPE_SIZE;

	return (i);
}

int pipe_read_2x ( void *buf, FILE *fp)
{
    if( !fp )return 0;
	
	int i=0;
	unsigned char *us = (unsigned char*) buf;
	
	if(fp->off2 >= fp->bsize) {
	
		fp->off2 = 0;
	}
	

    if(fp->off <= fp->off2 || ( (int)fp->off - (int)fp->off2 ) < 0){
        return 0;
    }
		
	for( i=0; i < PIPE_SIZE; i ++){
	
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2+i);
		*us ++ = *(unsigned char*)(fp->curp) &0xff;
		
	}

    fp->off2 += PIPE_SIZE;
	return (i);
}


