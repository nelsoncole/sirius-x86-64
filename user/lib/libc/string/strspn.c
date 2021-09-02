#include <string.h>

#define BITOP(A, B, OP) \
((A)[(size_t)(B)/(8*sizeof *(A))] OP (size_t)1<<((size_t)(B)%(8*sizeof *(A))))

size_t strspn(const char *s1, const char *s2)
{

	const char * a = s1;
	size_t byteset[32/sizeof(size_t)] = { 0 };

	if (!s2[0]) {
		return 0;
	}
	if (!s2[1]) {
		for (; *s1 == *s2; s1++);
		return s1-a;
	}

	for (; *s2 && BITOP(byteset, *(unsigned char *)s2, |=); s2++);
	for (; *s1 && BITOP(byteset, *(unsigned char *)s1, &); s1++);

	return s1-a;

}
