#ifndef __IPV4_H__
#define __IPV4_H__

typedef struct __ipv4_header{
    unsigned char   ihl:4;
    unsigned char   ver:4;
    unsigned char   tos;
    unsigned short  len;
    unsigned short  id;
    //unsigned short  flags:3;
    unsigned short  offset;//:13;
    unsigned char   ttl;
    unsigned char   protocol;
    unsigned short  checksum;
    unsigned int    src;
    unsigned int    dst;

} __attribute__ ((packed)) ipv4_header_t;


#endif
