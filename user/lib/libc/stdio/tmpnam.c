#include <stdio.h>
#include <string.h>
static char _tmpnam[L_tmpnam];


void sysgettmpnam(char *s)
{
	__asm__ __volatile__("int $0x72": :"d"(9),"D"(s));
}

char *tmpnam(char *s)
{
	if(!s) {
		sysgettmpnam(_tmpnam);
		return (_tmpnam);
	}
	
	sysgettmpnam(s);
	return s;
}
