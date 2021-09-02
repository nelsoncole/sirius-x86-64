#include <stdio.h>

extern int seek_r (FILE *fp, long num_bytes, int origin );

int fseek (FILE *fp, long num_bytes, int origin )
{

	return (seek_r (fp, num_bytes, origin ));
}
