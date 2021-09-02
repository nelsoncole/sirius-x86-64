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

static int vf(char c);

int vfprintf(FILE *fp,const char * fmt, va_list ap) {
  int index = 0;
  
  char c, *s;
  char buffer[256];
  
  int i;
  long l;
  double f;
  double d;
  
  unsigned int u;
  unsigned long lu;
  
  
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
            c = (char) va_arg (ap, int);
            fputc(c,fp);
          break;
          case 0x02:
            s = va_arg (ap, char*);
            if(s != 0) {
            	fputs(s,fp);
            } else 
            	fputs("(null)",fp);
          break;
          case 0x03:
            i = va_arg (ap, int);
            itoa (i,buffer);
            fputs (buffer,fp);
          break;
          case 0x04:
            u = va_arg (ap, unsigned int);
            utoa(u, buffer, 10);
            fputs(buffer,fp);
          break;
          case 0x05:
            u = va_arg (ap, unsigned int);
            i2hex(u, buffer,8);
            fputs(buffer,fp);
          break;
          case 0x06: // format usando notação cientifica
          case 0x07:
            f = va_arg (ap, double);
            ftoa(f, buffer, 6);
            fputs(buffer,fp);
          break;
          // long
          case 0x13:
            l = va_arg (ap, long);
            ltoa( l , buffer , 10);
            fputs (buffer,fp);
          break;
          case 0x14:
            lu = va_arg (ap, unsigned long);
            utoa( lu , buffer , 10);
            fputs(buffer,fp);
          break;
          case 0x15:
            lu = va_arg (ap, unsigned long);
            utoa( lu , buffer , 16);
            fputs(buffer,fp);
          break;
          case 0x16: // format usando notação cientifica
          case 0x17:
            d = va_arg (ap, double);
            ftoa(d, buffer, 6);
            fputs(buffer,fp);
          break;
        
          default:
            fputc('%',fp);
            fputc(fmt[index],fp);
          break;			
        }	
	
      } else fputc(fmt[index],fp);
       
    index ++;
  }
  return (0);
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





