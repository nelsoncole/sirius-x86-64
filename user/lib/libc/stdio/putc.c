#include <stdio.h>

extern int putc_r (int ch, FILE *fp);

int putc (int ch, FILE *fp) 
{
	return (putc_r (ch, fp));
}
