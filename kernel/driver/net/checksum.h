#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

typedef struct checksum_header
{
    unsigned int src;
    unsigned int dst;
    unsigned char rsved;
    unsigned char protocol;
    unsigned short len;

}__attribute__((packed)) checksum_header_t;


unsigned short net_checksum(const unsigned char *start, const unsigned char *end);



#endif
