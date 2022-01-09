#ifndef __UDP_H__
#define __UDP_H__

typedef struct __udp_header{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned short length;
    unsigned short checksum;
} __attribute__ ((packed)) udp_header_t;



#endif
