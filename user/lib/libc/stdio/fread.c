#include <stdio.h>

extern size_t read_r (void *buffer,size_t size, size_t count, FILE *fp);

size_t fread (void *buffer,size_t size, size_t count, FILE *fp)
{
	return (read_r (buffer, size, count, fp));
}
