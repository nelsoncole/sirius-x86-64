#include <string.h>
#include <ctype.h>


// strcasecmp -- Comparação de duas strings, sem diferenciar maiúsculas e minúsculas
// retorna < 0 se str1 é menor do que str2
// retorna > 0 se str1 é maior do que str2
// retorna = 0 se str1 é igual a str2
int strncasecmp (const char *str1,const char *str2,size_t len) 
{

	const unsigned char *p_s1 = (const unsigned char *) str1;
  	const unsigned char *p_s2 = (const unsigned char *) str2;
  	int d = 0;

  	for ( ; len != 0; len--) {

      		const int c1 = tolower(*p_s1++);
      		const int c2 = tolower(*p_s2++);
      		if (((d = c1 - c2) != 0) || (c2 == '\0'))
        	break;
    	}

  	return (d);
}
