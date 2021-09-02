#include <stdio.h>
#include <stdlib.h>
/*
 c == 		0x01
 s e S == 	0x02
 d e i == 	0x03
 u == 		0x04
 x e X == 	0x05
 g == 		0x06
 f == 		0x07
 
 ld e li == 	0x13
 lu == 	0x14
 lx e lX == 	0x15
 lg == 	0x16
 lf == 	0x17
 


*/
static int character(FILE *fp, char *dst)
{
	char *d = dst;
		
	fgets (d, 1, fp);
		
	return (int) 0;
}

static int str(FILE *fp, char *dst)
{
	char *s = dst;
	
	fgets (s, 256, fp);
	
	while( *s != ' ' && s < (dst + 256) && *s != '\n' ) s ++;
	
	*s = '\0';
	
	return  s == dst;
}

// size is 1 for char, 2 for short, 4 for int, and 8 for long
static int value(FILE *fp, void *dst, int size, int base)
{
	
	static char _buf[256];
	char *buf = _buf;
	char *end = buf + 256;
	
	fgets (buf, 256, fp);
	
	while( *buf == ' ' && *buf != '\0' && buf < end ) buf ++;
	
	if(buf >= end) { 
		buf = _buf;
		_buf[0] = '0';
		_buf[1] = '\0';	
	}
	
	if (size == 8) 
		*(long *) dst = (long)strtoul ( buf, (char **)0, base);
	else if (size == 4)
		*(int *) dst = (int) strtol( buf, (char **)0, base);
	else if (size == 2)
		*(short *) dst = (short) strtol( buf, (char **)0, base);
	else
		*(char *) dst = (char) strtol( buf, (char **)0, base);
		
	return 0;
}


static int fvalue(FILE *fp, void *dst, int size, int base)
{
	
	static char _buf[256];
	char *buf = _buf;
	char *end = buf + 256;
	
	fgets (buf, 256, fp);
	
	while( *buf == ' ' && *buf != '\0' && buf < end ) buf ++;
	
	if(buf >= end) { 
		buf = _buf;
		_buf[0] = '0';
		_buf[1] = '\0';	
	}
	
	if(size == 4) 
		*(float *) dst = (float) atof(buf);
	else if(size == 8)
		*(double *) dst = (double) atof(buf);
	else;
	
		
	return 0;
}

static int vf(char c);

int vfscanf(FILE *fp,const char * fmt, va_list ap) {
  int index = 0;
  int type = 0;

  while (fmt[index]) {
  
    type = 0;
    
    if ( (fmt[index] &0xff) == '%') {
        index ++;
        
        if( (fmt[index] &0xff) == 'l') {
          index ++;
          type = 0x10;
        }
        
        type |= vf( (fmt[index] &0xff) );
        
        switch (type) {
    	  case 0x01:
            character(fp, va_arg(ap, char *));
          break;
          case 0x02:
            str(fp, va_arg(ap, char *));
          break;
          case 0x03:
            value(fp, va_arg(ap, int *), 4, 10);
          break;
          case 0x04:
            value(fp, va_arg(ap, unsigned int *), 4, 10);
          break;
          case 0x05:
            //TODO x X
          break;
          case 0x06: // format usando notação cientifica
          case 0x07:
            fvalue(fp, va_arg(ap, float *), 4, 10);
          break;
          // long
          case 0x13:
            value(fp, va_arg(ap, long *), 8, 10);
          break;
          case 0x14:
            value(fp, va_arg(ap, unsigned long *), 8, 10);
          break;
          case 0x15:
            //TODO lx lX
          break;
          case 0x16: // format usando notação cientifica
          case 0x17:
            fvalue(fp, va_arg(ap, double *), 8, 10);
          break;
        
          default:
            // ignora
          break;			
        }	
	
      } else; //ignora
       
    index ++;
  }
  return (index);
}

static int vf(char c) {
  int type = 0;
  switch (c) {
    case 'c':
      type = 0x01;
    break;
    case 's':
    case 'S':
      type = 0x02;
    break;
    case 'd':
    case 'i':
      type = 0x03;
    break;
    case 'u':
      type = 0x04;
    break;
    case 'x':
    case 'X':
      type = 0x05;
    break;
    case 'g':
      type = 0x06;
    break;
    case 'f':
      type = 0x07;
    break;

    default:
      type = -1; 
    break;			
  }

  return type;
}

