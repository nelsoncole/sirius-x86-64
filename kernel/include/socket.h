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

    unsigned char   flags;
    unsigned int    length1;
    unsigned int    length2;
    unsigned long   buf1;
    unsigned long   buf2;

    unsigned char   num_client;   
    struct socket *client;

    struct socket *next;
}__attribute__((packed));


extern struct socket *current_saddr, *saddr_ready_queue;
void socket_server_transmit();
void socket_server_receive(unsigned int src_ip, unsigned int dest_ip, unsigned short src_port, unsigned short dest_port,
    const void *buffer, unsigned length);

int init_socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol);





#endif
