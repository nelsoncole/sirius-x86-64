#include <stdio.h>

extern int close_r(FILE *fp);
int fclose (FILE *fp) 
{
	return close_r(fp);
}

