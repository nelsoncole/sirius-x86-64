#include <stdio.h>

extern long int tell_r(FILE *fp);

long int ftell(FILE *fp)
{
	return tell_r(fp);

}
