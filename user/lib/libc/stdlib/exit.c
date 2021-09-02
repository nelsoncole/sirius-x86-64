#include <stdlib.h>
#include <stdio.h>


#undef        exit

void exit(int rc)
{

	__asm__ __volatile__("int $0x72"::"d"(8),"c"(6));
	for(;;);

}
