#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include "udp.h"
#include "ipv4.h"
#include "ether.h"

int udp_send(unsigned int src_address, unsigned int dst_address, unsigned short src_port, unsigned short dst_port, const void *data, size_t length)
{
    unsigned len = sizeof(ether_header_t) + sizeof(ipv4_header_t) + sizeof(udp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len );
    udp_header_t *udp = (udp_header_t*) (buf + sizeof(ipv4_header_t) + sizeof(ether_header_t));

    udp->length = htons(sizeof(udp_header_t) + length);
    udp->dst_port = htons(dst_port);
    udp->src_port = htons(src_port);
    udp->checksum = 0;

    // data
    if(data != 0 || length > 0 )
        memcpy((unsigned char*)udp + sizeof(udp_header_t), data, length);
    // 
    len = sizeof(udp_header_t) + length;
    ipv4_send(buf, IP_PROTOCOL_UDP, src_address, dst_address, len);

    free(buf);
 
    return length;
}


int udp_receive(void *data, size_t length){

    ethernet_package_descriptor_t *prd;
    ipv4_header_t *hdr;
    ether_header_t *eh;

    while(1){
        prd = get_ethernet_package();
        // pooling
        if(prd->flag) {
            continue;
        }

        eh = (ether_header_t*) prd->buf;
        hdr =(ipv4_header_t*) (prd->buf + sizeof(ether_header_t));
        if(htons(eh->type) == ET_IPV4) {
            if(hdr->protocol == IP_PROTOCOL_UDP){
                break;
            }

        }

    }

    hdr++;
    udp_header_t *udp = (udp_header_t*)hdr;
    udp++;
    unsigned char *payload = (unsigned char*)udp;
    udp--;


    int len;

    if(htons(udp->length) > length)
        len = length;
    else
        len = htons(udp->length);

    memcpy(data, payload , len);

    return len;    
}




