#include <pit.h>


void sleep(int ms)
{
	if(ms != 0) {
		timer_wait((FREQ *ms) / 1000);
	}
}
