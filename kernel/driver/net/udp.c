#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include "udp.h"
#include "ipv4.h"


int udp_send(unsigned int src_address, unsigned int dst_address, unsigned short src_port, unsigned short dst_port, const void *data, size_t length)
{
    unsigned len = sizeof(ipv4_header_t) + sizeof(udp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len );
    udp_header_t *udp = (udp_header_t*) (buf + sizeof(ipv4_header_t));

    unsigned char mac[SIZE_OF_MAC] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    if(dst_address != 0xFFFFFFFF){
        // get mac

        for(int i=0; i < 4; i++) {
            if( !arp_get_address(mac, router_ip) ) {

                if(i == 3) {
                    printf("ARP request error\n");
                    free(buf);
                    return 0; 
                }
                // broadcast
                mac[0] = 0xFF;mac[1] = 0xFF;mac[2] = 0xFF;
                mac[3] = 0xFF;mac[4] = 0xFF;mac[5] = 0xFF;
                arp_request((unsigned char *)&dst_address, mac);
                sleep(1000);
            }else {
                break;
            }
        }
    }else{ 
        // broadcast    
    }

    udp->length = htons(sizeof(udp_header_t) + length);
    udp->dst_port = htons(dst_port);
    udp->src_port = htons(src_port);
    udp->checksum = 0;

    // data
    memcpy((unsigned char*)udp + sizeof(udp_header_t), data, length);
    // 
    len = sizeof(udp_header_t) + length;
    ipv4_send(buf, IP_PROTOCOL_UDP, mac, src_address, dst_address, len);

    free(buf);
 
    return length;
}


int udp_receive(void *data, size_t length){

    ethernet_package_descriptor_t prd;
    ipv4_header_t *hdr;

    while(1){
        prd = get_ethernet_package();
        // pooling
        if(prd.flag) {
            continue;
        }

        hdr =(ipv4_header_t*) prd.buf;
        if(htons(hdr->eh.type) == ET_IPV4) {
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




