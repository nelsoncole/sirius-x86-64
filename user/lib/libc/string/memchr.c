#include <string.h>

void *memchr(const void *buf, int c, size_t n)
{
    unsigned char *p = (unsigned char *)buf;
    unsigned char *end = p + n;

    while (p != end)
    {
        if (*p == c)
        {
            return p;
        }

        ++p;
    }

    return 0;
}
