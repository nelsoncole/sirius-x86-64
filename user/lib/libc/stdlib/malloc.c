#include <stdlib.h>


#undef        malloc

extern void *__malloc_r(unsigned size, int type);

void *malloc(size_t size) {

	return __malloc_r(size, 0);
}
