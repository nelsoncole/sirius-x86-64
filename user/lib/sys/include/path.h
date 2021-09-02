#ifndef __PATH_H
#define __PATH_H

int setpath(const char *name);
int upath();

char *getpathname(char *dst, const char *src );
int path_count(const char *path);
int getfilename(char *filenane, char *path);

#endif
