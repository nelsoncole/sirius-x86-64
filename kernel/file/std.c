#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <file.h>

#include <io.h>

FILE *stdin, *stdout, *stderr;

FILE *_stdin, *_stdout, *_stderr;

FILE *std_open_file(const char *filename, const char *mode)
{

	char *m = (char*) mode;
	int flag = 0;
	
	if(memcmp(m, "stdin", 5) == 0) flag	= 0x2;
	else if(memcmp(m, "stdout", 6) == 0) flag	= 0x3;
	else if(memcmp(m, "stderr", 6) == 0) flag	= 0x4;
	else return 0;
	
	
	FILE *fp = (FILE *)malloc(sizeof(FILE));
	memset(fp,0,sizeof(FILE));
	
	fp->bsize 	= (unsigned) BUFSIZ;
	fp->buffer 	= (unsigned char*) malloc(BUFSIZ);
	fp->fsize	= 0;
	fp->mode 	= 0x2;
	fp->flags	= flag;
	
	memset(fp->buffer, 0, BUFSIZ);
	

	return (fp);
}


int std_file_close(FILE *fp)
{
    free(fp->buffer);
    free(fp);
    return 0;
}

int std_putc (int c, FILE *fp)
{
	
	if(fp->level >= fp->bsize || fp->off >= fp->bsize) {

		fp->off = fp->off2 = 0;
		fp->level = 0;
        fp->fsize = 0;
	}
	
	
	if(fp->flags == 4/*stderr*/){
		std_putc (c,_stdout);
    }

    if(fp->flags == 2/*stdin*/) {
        std_putc (c,_stdout);
    }

    if(fp->flags == 3/*stdout*/) {
        fp->curp = (unsigned char*)(fp->buffer + fp->off);
	    *(unsigned short*)(fp->curp) = c &0xffff;

	    // Update offset
	    fp->off += 2;
	    if(fp->off > fp->fsize ) fp->fsize += 2;

    }else {
	    fp->curp = (unsigned char*)(fp->buffer + fp->off);
	    *(unsigned char*)(fp->curp) = c &0xff;

	    // Update offset
	    fp->off += 1;
	    if(fp->off > fp->fsize ) fp->fsize += 1;
	}
	
	return (c);

}


int std_getc (FILE *fp)
{	
	int r = 0;
	if(!fp->fsize) return EOF;

	if(fp->flags == 2 ) { // stdin

		fp->off2++;
		while(fp->off < fp->off2) pause(); // wait
		
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2 - 1);
		r = *(unsigned char*)(fp->curp) &0xff;

	}else if(fp->flags == 4)  { // stderr


		if(!fp->off) return EOF;

		if(fp->off2 >= fp->off ) return EOF;

		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

		fp->off2++;

	}else if(fp->flags == 3){ // stdout

        if(!fp->off) return EOF;

		if(fp->off2 >= fp->off ) return EOF;

		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned short*)(fp->curp) &0xffff;

		fp->off2 += 2;
    }


	return (r);
}
