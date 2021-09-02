#include <stdio.h>


extern void rewind_r(FILE *fp);

void rewind (FILE *fp)
{
	rewind_r(fp);
}
