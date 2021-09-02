#include <stdlib.h>
#include <string.h>

#undef        getenv
extern char *pwd;

char *getenv(const char *name)
{

	if(strcmp(name, "PWD") == 0) {
		return ((char*)pwd);
	}			
	
	return ((char*)0);
}
