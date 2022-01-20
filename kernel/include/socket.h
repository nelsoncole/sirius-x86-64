#ifndef __SOCKET_H__
#define __SOCKET_H__

#define AF_UNSPEC   0
#define AF_LOCAL    1 // Machine-local comms
#define AF_INET     2 // IPv4
#define PF_INET6    3 // IPv6
#define PF_PACKET	4 // Low level packet interface

#define SOCK_STREAM       	1
#define SOCK_DGRAM        	2
#define SOCK_SEQPACKET    	3

struct socket
{
    int id;
    int domain;
    int type;
    int protocol;
    unsigned short  src_port;
    unsigned int    src_ip;

    unsigned short  dest_port;
    unsigned int    dest_ip;

    // For TCP
    unsigned int    seq;
    unsigned int    ack;
    unsigned char   protocol_flags;
    unsigned short  src_win;
    unsigned short  dst_win;

    // cache
    unsigned int    seq_x;
    unsigned int    ack_x;

    unsigned char   flags;
    unsigned int    length1;
    unsigned int    length2;
    unsigned long   buf1;
    unsigned long   buf2;

    unsigned char   num_client;   
    struct socket *client;

    struct socket *next;
}__attribute__((packed));

struct socket_receive_row{
    unsigned char   status;

    unsigned char   protocol;
    unsigned short  src_port;
    unsigned int    src_ip;

    unsigned short  dest_port;
    unsigned int    dest_ip;

    // For TCP
    unsigned int    seq;
    unsigned int    ack;
    unsigned char   flags;
    unsigned short  win;

    unsigned char   *buffer;
    unsigned        length;
}__attribute__ ((packed));


extern struct socket *current_saddr, *saddr_ready_queue;
void socket_server_transmit();
int socket_server_receive(int origem, int protocol, unsigned int src_ip, unsigned int dest_ip, unsigned short src_port, unsigned short dest_port,
    const void *buffer, unsigned length, unsigned int seq, unsigned int ack, unsigned char flags);
void socket_execute_row();

int init_socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol);





#endif
