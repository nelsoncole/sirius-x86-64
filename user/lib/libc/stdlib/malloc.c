#include <stdlib.h>


#undef        malloc

extern void *__malloc_r(unsigned size);

void *malloc(size_t size) {

	return __malloc_r(size);
}
