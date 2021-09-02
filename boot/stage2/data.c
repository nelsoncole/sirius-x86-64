#include <io.h>

unsigned char dv_num;
unsigned int dv_uid;
unsigned int heap = 0;

void wait(int n)
{

	int count = n/100;
	while(--count)__asm__ __volatile__("out %%al,$0x80"::);
}
