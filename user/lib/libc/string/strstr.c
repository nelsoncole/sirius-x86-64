#include <string.h>

#include <stddef.h>


char *strstr (const char *s1, const char *s2)
{
	const size_t len = strlen (s2);
	while (*s1) {
      		if (!memcmp (s1, s2, len))
			return (char *)s1;
      		++s1;
    	}
  
  	return (0);
}
