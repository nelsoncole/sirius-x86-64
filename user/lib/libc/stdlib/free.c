#include <stdlib.h>

extern void __free_r(void *ptr);
#undef        free

void free(void *ptr) {
	__free_r(ptr);
	
}
