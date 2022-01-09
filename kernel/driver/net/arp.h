#ifndef __ARP_H__
#define __ARP_H__

#include "ether.h"

typedef struct __arp_header{
    ether_header_t  eh;
    unsigned short  hardware_type;
    unsigned short  protocol_type;
    unsigned char   hardware_size;
    unsigned char   protocol_size;
    unsigned short  operation;

    unsigned char   source_mac[SIZE_OF_MAC];
    unsigned char   source_ip[SIZE_OF_IP];

    unsigned char   dest_mac[SIZE_OF_MAC];
    unsigned char   dest_ip[SIZE_OF_IP];

} __attribute__ ((packed)) arp_header_t;

typedef struct __arp_cache{
    unsigned char   mac[SIZE_OF_MAC];
    unsigned char   ip[SIZE_OF_IP];
} __attribute__ ((packed)) arp_cache_t;

extern arp_cache_t arp_cache[1024];

void arp_hexdump(arp_header_t *arp);

#endif
