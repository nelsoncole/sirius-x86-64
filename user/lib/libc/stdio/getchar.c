#include <stdio.h>

extern int getc_r (FILE *fp);
extern int putc_r (int ch, FILE *fp);

int getchar ()
{	
	int c = getc_r (stdin);

	if(c != EOF) {
	
		putc_r (c, stdout);
	}
	return (c);

}

