#include <stdio.h>
#include <string.h>

int rename(const char *old, const char *new)
{
	FILE *f = fopen(old, "r+");
	if(f == 0) {
	
		return EOF;
	}
	
	int r = 0;
	int len = strlen(new);
	
	if(len > FILENAME_MAX) r = - EOF;
	else strcpy(f->fname, new);
	
	f->flags |= 0x80 | 0x40;
	fclose(f);
	return r;
}
