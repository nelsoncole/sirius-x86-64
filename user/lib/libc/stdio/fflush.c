#include <stdio.h>

extern int flush_r(FILE *fp);

int fflush(FILE *fp) {

	return flush_r(fp);

}
