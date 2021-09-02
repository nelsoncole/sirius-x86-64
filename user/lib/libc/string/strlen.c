#include <string.h>

size_t strlen(const char *s)
{
	char *tmp = (char*)s;
	
	while(*tmp != '\0')tmp++;

	return (size_t)(tmp - s);
}
