#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ether.h"
#include "ipv4.h"
#include "tcp.h"
#include "checksum.h"

int tcp_send(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq, unsigned char flags, const void *data, size_t length)
{
    unsigned len = sizeof(ether_header_t) + sizeof(ipv4_header_t) + sizeof(tcp_header_t) + length + 40;
    unsigned char *buf = (unsigned char*) malloc( len );
    tcp_header_t *tcp = (tcp_header_t*) (buf + sizeof(ipv4_header_t) + sizeof(ether_header_t));

    int rcvNxt = 0;
    int HLEN = 0;

    tcp->dst_port = htons(dst_port);
    tcp->src_port = htons(src_port);
    tcp->seq = htonl(seq);
    tcp->ack = htonl( flags & TCP_ACK ? rcvNxt : 0);
    tcp->off = HLEN << 2;
    tcp->flags = flags;
    tcp->window_size = htons(TCP_WINDOW_SIZE); 
    tcp->checksum = htons(0);
    tcp->urgent = htons(0);

    unsigned char *options = (unsigned char*)tcp;
    options += sizeof(tcp_header_t);

    // data
    memcpy(options, data, length);

    unsigned long start = (unsigned long)tcp;
    unsigned long end = start + (sizeof(tcp_header_t) + length);

    // Pseudo Header
    checksum_header_t *phdr = (checksum_header_t *)(start - sizeof(checksum_header_t));
    phdr->src = src_port;
    phdr->dst = dst_port;
    phdr->rsved = 0;
    phdr->protocol = IP_PROTOCOL_TCP;
    phdr->len = htons(end - start);

    // Checksum
    unsigned short checksum = net_checksum( (const unsigned char *)(start - sizeof(checksum_header_t)), (const unsigned char *)end);
    tcp->checksum = htons(checksum);

    // 
    len = sizeof(tcp_header_t) + length;
    ipv4_send(buf, IP_PROTOCOL_TCP, src_address, dst_address, len);

    free(buf);
 
    return length;
}
