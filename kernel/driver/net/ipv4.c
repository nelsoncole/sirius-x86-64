#include <ethernet.h>
#include <string.h>
#include <stdio.h>

#include "ipv4.h"
#include "ether.h"


unsigned char *ipv4_cache;
unsigned int ipv4_count = 0;
int ipv4_send(void *buf, unsigned char protocol, unsigned int src_address, unsigned int dst_address, unsigned length)
{

    unsigned long addr = (unsigned long)buf;
    ether_header_t *eh = (ether_header_t *)addr;
    ipv4_header_t *hdr = (ipv4_header_t*)(addr + sizeof(ether_header_t));


    unsigned char mac[SIZE_OF_MAC] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    
    if(dst_address != 0xFFFFFFFF){
        if(get_hardwere_ethernet(mac)) {
            printf("Hardware indefinido\n");
                    return 0;
        } 
    }

    // Ethernet header
    fillMac(eh->dst, mac);
    fillMac(eh->src, default_ethernet_device.mac);
    eh->type = htons(ET_IPV4);


    unsigned len = length + sizeof(ipv4_header_t);

    // Ipv4 header
    hdr->ihl = 5;
    hdr->ver = 4;
    hdr->tos = 0;
    hdr->len = htons(len);
    hdr->id = htons(ipv4_count++);
    hdr->offset = htons(0x4000);
    hdr->ttl = 63;
    hdr->protocol = protocol;
    hdr->checksum = 0;
    hdr->src = src_address;
    hdr->dst = dst_address;

    // Checksum
    unsigned char *start = (unsigned char *)hdr;
    unsigned char *end = (unsigned char *)(start + sizeof(ipv4_header_t));
    unsigned short checksum = net_checksum(0, 0,(const unsigned char *)start, (const unsigned char *)end);
    hdr->checksum = htons(checksum);

    // Hardware
    if(send_ethernet_package( (void*)addr,length + sizeof(ipv4_header_t) + sizeof(ether_header_t)) ) 
        return 1;

    memcpy(ipv4_cache, (void*)addr , length + sizeof(ipv4_header_t) + sizeof(ether_header_t));

    return 0;
}
