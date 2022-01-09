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
            if( !arp_get_address(mac, (unsigned char *)&dst_address) ) {
                // broadcast
                mac[0] = 0xFF;mac[1] = 0xFF;mac[2] = 0xFF;
                mac[3] = 0xFF;mac[4] = 0xFF;mac[5] = 0xFF;
                arp_request((unsigned char *)&dst_address, mac);
                sleep(1000);
            }else {
                break;
            }

            if(i == 3) {
                for(int t=0; t < 6; t++)printf("%x:",mac[t]);
                printf("arp request error\n");
                free(buf);
                return 0; 
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
