//
#include <stdio.h>

#include <time.h>

void debug_o(char *s, int a, int b )
{
	printf("Dubug: %s %x %x\n", s, a, b);
  	time_t	timer;
	struct tm *h;
	
	h = gmtime(&timer);
  	int sec = h->tm_sec;
  	while(1){
  		if(sec != h->tm_sec)
  			break;
	}
}
