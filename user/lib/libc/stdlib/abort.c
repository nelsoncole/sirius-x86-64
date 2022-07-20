#include <stdlib.h>


#undef        abort

extern void sys_exit(int rc);
void abort(void)
{
	sys_exit(1);
}
