#include <stdlib.h>
#include <stdio.h>


#undef        exit

extern void sys_exit(int rc);
void exit(int rc)
{
	sys_exit(rc);
}
