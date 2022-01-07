#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

int udp_send(unsigned int dst_address, unsigned short dst_port, const void *data, size_t length)
{
    unsigned len = sizeof(ipv4_header_t) + sizeof(udp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len );
    udp_header_t *udp = (udp_header_t*) (buf + sizeof(ipv4_header_t));

    unsigned char mac[SIZE_OF_MAC] ={0,0,0,0,0,0};
    unsigned int src_address = 0;
    fillIP((unsigned char*)&src_address, default_ethernet_device.client_ip);


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
            return 0; 
        }
    }


    unsigned short src_port = 20001;

    udp->length = switch_endian16(sizeof(udp_header_t) + length);
    udp->dst_port = switch_endian16(dst_port);
    udp->src_port = switch_endian16(src_port);
    udp->checksum = 0;

    // data
    memcpy((unsigned char*)udp + sizeof(udp_header_t), data, length);
    // 
    len = sizeof(udp_header_t) + length;
    ipv4_send(buf, IP_PROTOCOL_UDP, mac, src_address, dst_address, len);

    free(buf);
 
    return length;
}
