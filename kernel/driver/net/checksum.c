#include <ethernet.h>
#include "checksum.h"

unsigned short net_checksum(const unsigned char *phdr, int phdr_len, const unsigned char *start, const unsigned char *end)
{

    unsigned int checksum = 0;
    unsigned int len = phdr_len;
    unsigned short *p = (unsigned short *)phdr;

    if(!phdr) goto no;

    // acc
    // Pseudo Head
    while (len > 1) {
        checksum += *p++;
        len -= 2;
    }

no:

    len = end - start;
    p = (unsigned short *)start;
    while (len > 1) {
        checksum += *p++;
        len -= 2;
    }

    if (len != 0) {
        checksum += *(unsigned char *)p;
    }


    checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum += (checksum >> 16);

    unsigned short final = ~checksum;

    return htons(final);
}
