#include <stdio.h>

extern int getc_r (FILE *fp);
extern int putc_r (int ch, FILE *fp);

int getc (FILE *fp)
{	
	if(!fp) return -1;
	
	int c = getc_r (fp);

	if(fp->flags == 2) { // stdin
	
		putc_r (c, stdout);
	}
	return (c);

}

