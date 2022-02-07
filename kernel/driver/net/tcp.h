#ifndef __TCP_H__
#define __TCP_H__

// Options
#define TCP_OPT_END     0
#define TCP_OPT_NOP     1
#define TCP_OPT_MSS     2
#define TCP_OPT_SACK    4

#define TCP_WINDOW_SIZE 4096

typedef struct __tcp_header
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned int seq;
    unsigned int ack;
    unsigned char off;
    unsigned char flags;
    unsigned short window_size;
    unsigned short checksum;
    unsigned short urgent;

} __attribute__ ((packed)) tcp_header_t;


struct tcp_connect {
    unsigned int src_ip;
    unsigned int dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned int seq;
    unsigned int ack;
    unsigned int pack;
    unsigned char flags;
    unsigned char status;
    unsigned char busy;
    unsigned char rsv;

}__attribute__ ((packed));

#endif
