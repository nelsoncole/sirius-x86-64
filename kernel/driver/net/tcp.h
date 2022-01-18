#ifndef __TCP_H__
#define __TCP_H__

// Flags
#define TCP_FIN     (1 << 0)
#define TCP_SYN     (1 << 1)
#define TCP_RST     (1 << 2)
#define TCP_PSH     (1 << 3)
#define TCP_ACK     (1 << 4)
#define TCP_URG     (1 << 5)

// Options
#define TCP_OPT_END     0
#define TCP_OPT_NOP     1
#define TCP_OPT_MSS     2
#define TCP_OPT_SACK    4

#define TCP_WINDOW_SIZE 8192

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

#endif
