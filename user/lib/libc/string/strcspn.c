#include <string.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(X) (((X)-ONES) & ~(X) & HIGHS)

#define BITOP(A, B, OP) \
 ((A)[(size_t)(B)/(8*sizeof *(A))] OP (size_t)1<<((size_t)(B)%(8*sizeof *(A))))


char * strchrnul(const char * s, int c) {
	size_t * w;
	size_t k;

	c = (unsigned char)c;
	if (!c) {
		return (char *)s + strlen(s);
	}

	for (; (unsigned long)s % ALIGN; s++) {
		if (!*s || *(unsigned char *)s == c) {
			return (char *)s;
		}
	}

	k = ONES * c;
	for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
	for (s = (void *)w; *s && *(unsigned char *)s != c; s++);
	return (char *)s;
}

size_t strcspn(const char * s1, const char * s2) {
	const char *a = s1;
	if (s2[0] && s2[1]) {
		size_t byteset[32/sizeof(size_t)] = { 0 };
		for (; *s2 && BITOP(byteset, *(unsigned char *)s2, |=); s2++);
		for (; *s1 && !BITOP(byteset, *(unsigned char *)s1, &); s1++);
		return s1-a;
	}
	return strchrnul(s1, *s2)-a;
}
