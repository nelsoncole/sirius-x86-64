#include <stdio.h>
#include <string.h>


void puts(const char* str)
{
	
	int i;
	if(!str)return;
	 
	for(i=0;i <strlen(str);i++) putc (str[i],stdout);
}
