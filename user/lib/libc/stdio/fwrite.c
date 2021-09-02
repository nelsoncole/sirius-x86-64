#include <stdio.h>

extern size_t write_r (const void *buffer, size_t size, size_t count, FILE *fp);
size_t fwrite (const void *buffer, size_t size, size_t count, FILE *fp)
{
	return (write_r (buffer, size, count, fp));
}
