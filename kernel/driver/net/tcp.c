#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ether.h"
#include "ipv4.h"
#include "tcp.h"
#include "checksum.h"

int tcp_send(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq, unsigned int ack, unsigned char flags, const void *data, size_t length)
{
    unsigned len = sizeof(ether_header_t) + sizeof(ipv4_header_t) + sizeof(tcp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len + 40);
    tcp_header_t *tcp = (tcp_header_t*) (buf + sizeof(ipv4_header_t) + sizeof(ether_header_t));

    tcp->dst_port = htons(dst_port);
    tcp->src_port = htons(src_port);
    tcp->seq = htonl(seq);
    tcp->ack = htonl( flags & TCP_ACK ? ack : 0);
    tcp->off = 0 << 2;
    tcp->flags = flags;
    tcp->window_size = htons(TCP_WINDOW_SIZE); 
    tcp->checksum = htons(0);
    tcp->urgent = htons(0);

    unsigned char *start = (unsigned char *)tcp;
    
    unsigned char *options = start;
    options += sizeof(tcp_header_t);

    if (flags & TCP_SYN) {

        options[0] = TCP_OPT_MSS; // Maximum Segment Size (2)
        options[1] = 4;
        *(unsigned short *)(options + 2) = htons(1460);
        options += options[1];
        /*
        options[0] = TCP_OPT_SACK; // SACK Permitted
        options[1] = 2;
        options += options[1]; */
        

    }

    // Option End
    while ((options - start) & 3)
    {
        *options++ = 0;
    }

    int hlen = (options - start);
    tcp->off = hlen << 2;

    // data
    if(data != 0 || length > 0 )
        memcpy(options, data, length);

    unsigned char *end = (unsigned char *)(start + (hlen + length));

    // Pseudo Header
    checksum_header_t bphdr[1];
    checksum_header_t *phdr = (checksum_header_t *)&bphdr;
    phdr->src = src_address;
    phdr->dst = dst_address;
    phdr->rsved = 0;
    phdr->protocol = IP_PROTOCOL_TCP;
    phdr->len = htons(end - start);

    // Checksum
    unsigned short checksum = net_checksum((const unsigned char*)phdr, sizeof(checksum_header_t),(const unsigned char *)start, (const unsigned char *)end);
    tcp->checksum = htons(checksum);

    // 
    len = hlen + length;
    ipv4_send(buf, IP_PROTOCOL_TCP, src_address, dst_address, len);

    free(buf);
 
    return length;
}
