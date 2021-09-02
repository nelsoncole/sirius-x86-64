
#include <string.h>
//#include <stddef.h>
#include <stdlib.h>

char *strdup(const char *s) 
{

  	size_t len = strlen (s) + 1;
  	char *result = (char*) malloc (len);
  	if (result == (char*) 0)
    		return (char*) 0;
  	
  	return (char*) memcpy (result, s, len);
}
