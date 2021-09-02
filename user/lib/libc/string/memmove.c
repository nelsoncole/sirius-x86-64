#include <string.h>

void *memmove(void *s1, const void *s2, size_t n)
{
    unsigned char *p = (unsigned char *)s2;
    unsigned char *q = (unsigned char *)s1;
    unsigned char *end = p + n;

    if (q > p && q < end)
    {
        p = end;
        q += n;
        while (p != s2)
        {
            *--q = *--p;
        }
    }
    else
    {
        while (p != end)
        {
            *q++ = *p++;
        }
    }
    return s1;
}
