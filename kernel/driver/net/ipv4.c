#include <ethernet.h>

#include "ipv4.h"

int ipv4_send(void *buf, unsigned char protocol, unsigned char *mac, unsigned int src_address, unsigned int dst_address, unsigned length)
{
    ipv4_header_t *hdr = (ipv4_header_t*)buf;

    // Ethernet header
    fillMac((unsigned char*)&hdr->eh.dst, mac);
    fillMac((unsigned char*)&hdr->eh.src, (unsigned char*)&default_ethernet_device.mac);
    hdr->eh.type = ETHERNET_TYPE_IP4;


    unsigned len = length + (sizeof(ipv4_header_t) - sizeof(ether_header_t));

    // Ipv4 header
    hdr->ihl = 5;
    hdr->ver = 4;
    hdr->tos = 0;
    hdr->len = htons(len);
    hdr->id = htons(1);
    hdr->offset = htons(0x4000);
    hdr->ttl = 64;
    hdr->protocol = protocol;
    hdr->checksum = 0;
    hdr->src = src_address;
    hdr->dst = dst_address;

    unsigned long checksum = 0;
    checksum += 0x4500;
    checksum += len;
    checksum += 1;
    checksum += 0x4000;
    checksum += 0x4000 + protocol;
    checksum += htons((src_address >> 16) & 0xFFFF);
    checksum += htons(src_address & 0xFFFF); 
    checksum += htons((dst_address >> 16) & 0xFFFF);
    checksum += htons(dst_address & 0xFFFF);
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    hdr->checksum = htons((unsigned short) (~checksum));

    // Hardware
    if(send_ethernet_package( hdr,length + sizeof(ipv4_header_t))) 
        return 1;

    return 0;
}
