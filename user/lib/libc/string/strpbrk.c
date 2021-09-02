#include <string.h>

/*

 Retorna o primeiro caractere que está em ambas strings recebidas, ou NULL se não existe caractere comum.

*/
char *strpbrk(const char *s1, const char *s2)
{
	s1 += strcspn(s1, s2);
	
	return *s1 ? (char *)s1 : 0;
}

