#include <stdlib.h>

void *calloc(size_t nmemb, size_t size) 
{
	return malloc(nmemb*size);
}
