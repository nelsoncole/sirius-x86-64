#include <stdio.h>


extern int remove_file(const char *path);
int remove (const char *path)
{
	return remove_file(path);
}
