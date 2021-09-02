#ifndef __PIPE_H
#define __PIPE_H

extern FILE *__pipe__;
extern int pipe_write ( void *buf, FILE *fp);
extern int pipe_read ( void *buf, FILE *fp);

extern int pipe_read_2x ( void *buf, FILE *fp);


typedef struct __pipe
{
	unsigned short id;
	unsigned short r1;
	unsigned short r2;
	unsigned short r3;
	unsigned long  r4;
	
}__attribute__ ((packed)) __pipe_t;


#endif

