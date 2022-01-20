#ifndef __SOCKET_H
#define __SOCKET_H

#define SOCK_STREAM       	1
#define SOCK_DGRAM        	2
#define SOCK_SEQPACKET    	3


// ...
#define SOCK_PACKET       	10    /* linux specified 		    */
#define SOCK_MAX            (128)


// Supported address families.
#define AF_UNSPEC   0
#define AF_LOCAL    1 // Machine-local comms
#define AF_INET     2 // IPv4
#define PF_INET6    3 // IPv6
#define PF_PACKET	4 // Low level packet interface

/* Flags we can use with send/ and recv. 
   Added those for 1003.1g not all are supported yet */
 
#define MSG_OOB        1
#define MSG_PEEK       2
#define MSG_DONTROUTE  4
#define MSG_TRYHARD    4     /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC     8
#define MSG_PROBE      0x10  /* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC      0x20
#define MSG_DONTWAIT   0x40    /* Nonblocking io		 */
#define MSG_EOR        0x80    /* End of record */
#define MSG_WAITALL    0x100   /* Wait for a full request */
#define MSG_FIN        0x200
#define MSG_SYN        0x400
#define MSG_CONFIRM    0x800   /* Confirm path validity */
#define MSG_RST        0x1000
#define MSG_ERRQUEUE   0x2000  /* Fetch message from error queue */
#define MSG_NOSIGNAL   0x4000  /* Do not generate SIGPIPE */
#define MSG_MORE       0x8000  /* Sender will send more */
#define MSG_EOF        MSG_FIN


#include <ssize_t.h>
#include <size_t.h>

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

int accept(int socket, struct sockaddr *address,
             socklen_t *address_len);
int bind(int socket, const struct sockaddr *address,
             socklen_t address_len);
int connect(int socket, const struct sockaddr *address,
             socklen_t address_len);
int listen(int socket, int backlog);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t length,
             int flags, struct sockaddr *address, socklen_t *address_len);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
             const struct sockaddr *dest_addr, socklen_t dest_len);
int shutdown(int socket, int how);
int socket(int domain, int type, int protocol);



#endif
