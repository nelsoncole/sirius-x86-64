#include <delay.h>

void mdelay(int count)
{

	for(int i=0; i < count*10000; i++)__asm__ __volatile__("nop;");

}

void udelay(int count)
{

	for(int i=0; i < count*10000; i++)__asm__ __volatile__("nop;");

}
