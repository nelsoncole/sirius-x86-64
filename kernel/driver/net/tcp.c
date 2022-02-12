#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ether.h"
#include "ipv4.h"
#include "tcp.h"
#include "checksum.h"

#include <mm.h>

struct tcp_connect *queue_conn;

int init_tcp(){
    unsigned long addr = 0;
    // 32 KiB
    alloc_pages(0, 8, (unsigned long*)&addr);
    queue_conn = (struct tcp_connect *)addr;
    memset(queue_conn,0, 8*0x1000);
}

int tcp_send(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq, unsigned int ack, unsigned char flags, const void *data, size_t length)
{
    unsigned len = sizeof(ether_header_t) + sizeof(ipv4_header_t) + sizeof(tcp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len + 40);
    tcp_header_t *tcp = (tcp_header_t*) (buf + sizeof(ipv4_header_t) + sizeof(ether_header_t));

    tcp->dst_port = htons(dst_port);
    tcp->src_port = htons(src_port);
    tcp->seq = htonl(seq);
    tcp->ack = htonl(ack);
    tcp->off = 0 << 2;
    tcp->flags = flags;
    tcp->window_size = htons(TCP_WINDOW_SIZE); 
    tcp->checksum = htons(0);
    tcp->urgent = htons(0);

    unsigned char *start = (unsigned char *)tcp;
    
    unsigned char *options = start;
    options += sizeof(tcp_header_t);

    if (flags & TCP_SYN || 1) {

        options[0] = TCP_OPT_MSS; // Maximum Segment Size (2)
        options[1] = 4;
        *(unsigned short *)(options + 2) = htons(1460);
        options += options[1];
       
        options[0] = TCP_OPT_SACK; // SACK Permitted
        options[1] = 2;
        options += options[1];
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

int tcp_connect(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port){

    struct tcp_connect *conn = queue_conn;

    int i;
    for( i = 1024; i > 0; i--) {
        if(!conn->busy){
            conn->busy = 1;
            break;
        }

        conn++;   
    }

    if (i <= 0) {
		return 1;
	}


    conn->src_ip = src_address;
    conn->dst_ip = dst_address;
    conn->src_port = src_port;
    conn->dst_port = dst_port;
    conn->seq = 0;
    conn->ack = 0;
    conn->pack = 0;
    conn->flags = TCP_SYN;
    conn->status = 0;
    conn->busy = 1;

    tcp_send(conn->src_ip, conn->dst_ip, conn->src_port, conn->dst_port, conn->seq, conn->ack, conn->flags, 0, 0);

    conn->seq += 1;
    
    return 0;
}

int tcp_ack(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq,
unsigned int ack, unsigned char flags, size_t len){

    struct tcp_connect *conn = queue_conn;
    
    int i;
    for( i = 1024; i > 0; i--) {
        if(conn->src_port == dst_port &&  conn->busy == 1 && conn->status == 0 ){
            break;
        }

        conn++;   
    }

    if (i <= 0) {
		return 1;

	}if(conn->seq != ack){
        return 1;

    }
   
    if( conn->ack >= (seq + (flags & TCP_PSH ? len : 1)) ){
        // ACK
        // Retransmission
        conn->flags = TCP_ACK;
        tcp_send(conn->src_ip, conn->dst_ip, conn->src_port, conn->dst_port, conn->seq, conn->ack, conn->flags, 0, 0);
        return 1;
    }
    conn->pack = ack;

    conn->ack = seq;
    conn->ack += (flags & TCP_PSH ? len : 1);

    conn->flags = TCP_ACK;
    
    tcp_send(conn->src_ip, conn->dst_ip, conn->src_port, conn->dst_port, conn->seq, conn->ack, conn->flags, 0, 0);

    if(flags&TCP_FIN){
        conn->flags = TCP_FIN | TCP_ACK;
        conn->flags = tcp_send(conn->src_ip, conn->dst_ip, conn->src_port, conn->dst_port, conn->seq, conn->ack, conn->flags, 0, 0);
        conn->seq += 1;
        //conn->busy == 0;
        conn->status = 1;
        // send TCP_FIN
        return 2;
    }


    return 0;
}

int tcp_send_payload(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned char flags, const void *data, size_t len){

    struct tcp_connect *conn = queue_conn;

    int i;
    for( i = 1024; i > 0; i--) {
        if(conn->src_port == src_port &&  conn->busy == 1 && conn->status == 0 ){
            break;
        }

        conn++;   
    }

    if (i <= 0) {
		return 1;
	}

    conn->flags = flags;

    tcp_send(conn->src_ip, conn->dst_ip, conn->src_port, conn->dst_port, conn->seq, conn->ack, conn->flags, data, len);
    conn->seq += len;

    if(flags&TCP_FIN){
        // TCP_FIN
        conn->status = 1;
    }

    return 0;
}

void tcp_finish(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int ack){
    struct tcp_connect *conn = queue_conn;
    
    int i;
    for( i = 1024; i > 0; i--) {
        if(conn->src_port == dst_port &&  conn->busy == 1 && conn->status == 1 ){
            break;
        }

        conn++;   
    }

    if (i <= 0) {
		return;
	}

    if(conn->seq != ack){
        return;

    }

    conn->busy = 0;

}
