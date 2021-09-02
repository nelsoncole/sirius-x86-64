#include <string.h>


// compara os primeiros valores de bytes no limite de n
// retorna < 0 se s1 é menor do que s2
// retorna > 0 se s1 é maior do que s2
// retorna = 0 se s1 é igual a s2

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p_s1 = (const unsigned char *) s1;
  	const unsigned char *p_s2 = (const unsigned char *) s2;
  	int d = 0;

  	for ( ; n != 0; n--) {

      		const int c1 = *p_s1++;
      		const int c2 = *p_s2++;
      		if (((d = c1 - c2) != 0) || (c2 == '\0'))
        	break;
    	}

  	return (d);
	
}
