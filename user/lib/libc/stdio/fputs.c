#include <stdio.h>
#include <string.h>


int fputs (const char *str,FILE *fp)
{
	int i;
	if(!str)return 0;
	 
	for(i=0;i <strlen(str);i++) putc (str[i],fp);

	return i;
}

