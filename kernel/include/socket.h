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

struct socket_client
{
    int id;
    int domain;
    int type;
    int protocol;
    unsigned short  address_port;
    unsigned int    address_ip;

    unsigned char   flags;
    unsigned int    length;
    unsigned long   buf;

    struct socket_client *next;
}__attribute__((packed));


struct socket
{
    int id;
    int domain;
    int type;
    int protocol;
    unsigned short  address_port;
    unsigned int    address_ip;

    unsigned char   flags;
    unsigned int    length;
    unsigned long   buf;

    unsigned char   num_client;   
    struct socket_client *client;

    struct socket *next;
}__attribute__((packed));

int init_socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol);



#endif
