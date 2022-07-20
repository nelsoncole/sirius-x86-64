#ifndef __STRING_H
#define __STRING_H

#include "size_t.h"
#define NULL ((void *)0)

extern size_t strlen(const char *s);

extern void *memset(void *s, char val, size_t count);

extern char *strcpy(char* s1, const char* s2);

extern int strncpy(char *s1,const char *s2,size_t count);

extern int strcmp (const char* s1, const char* s2);

extern int strncmp (const char* s1,const char* s2,size_t count);

extern void *memcpy(void * restrict s1, const void * restrict s2, size_t n);

extern char *strcat (char *dest, const char *src);

extern int strcasecmp (const char *str1,const char *str2);

extern int strncasecmp (const char *str1,const char *str2,size_t len);

extern int memcmp(const void *s1, const void *s2, size_t n);

extern char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

extern size_t strspn(const char *s1, const char *s2);
extern size_t strcspn(const char *s1, const char *s2);

extern char *strtok(char * restrict s1,const char * restrict s2);

extern char *strpbrk(const char *s1, const char *s2);
extern char *strsep(char **stringp, const char *delim);

extern char *strstr (const char *s1, const char *s2);
extern char *strdup(const char *s);

extern void *memmove(void *dst, const void *src, size_t n);

extern char *strerror(int errnum);
extern int strcoll(const char *a, const char *b);

extern void *memchr(const void *buf, int c, size_t n);
// FIXME sem suport






#endif
