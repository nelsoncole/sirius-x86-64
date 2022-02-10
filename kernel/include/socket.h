#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <ssize_t.h>

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
    unsigned char   protocol_flags;
    unsigned short  src_win;
    unsigned short  dst_win;

    //
    unsigned char   connect;

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
void *socket_address(int socket);


typedef unsigned char sa_family_t;
typedef unsigned int socklen_t;

struct sockaddr
{
    sa_family_t sin_family;
    char sa_data[18];

}__attribute__((packed));

struct msghdr
{
    void         *msg_name;
    socklen_t     msg_namelen;
    struct iovec *msg_iov;
    int           msg_iovlen;
    void         *msg_control;
    socklen_t     msg_controllen;
    int           msg_flags;

}__attribute__((packed));

struct cmsghdr
{
	socklen_t	cmsg_len;
	int	cmsg_level;
	int	cmsg_type;

}__attribute__((packed));

struct linger
{
	int	l_onoff;
	int	l_linger;

}__attribute__((packed));

int init_socket(int domain, int type, int protocol);
int socket(int domain, int type, int protocol);
int bind(int socket, const struct sockaddr *address,
             socklen_t address_len);
ssize_t recvfrom(int socket, void *buffer, size_t length,
             int flags, struct sockaddr *address, socklen_t *address_len);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
             const struct sockaddr *dest_addr, socklen_t dest_len);





#endif
