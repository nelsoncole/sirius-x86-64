#ifndef __INET_H__
#define __INET_H__

#include <socket.h>
//netinet/in.h
#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_IGMP    2
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_IPV6    41

typedef unsigned int in_addr_t;

struct in_addr
{
	in_addr_t s_addr;
};

struct sockaddr_in
{
	sa_family_t	sin_family;
	unsigned short	sin_port;
	struct in_addr	sin_addr;
};

#define INADDR_ANY	0x00000000
#define INADDR_BROADCAST	0xFFFFFFFF

#define IPPORT_RESERVED	1024
#define IPPORT_UNRESERVED	0xC000


#define INET_ADDRSTRLEN 	16

static inline unsigned short htons(unsigned short nb)
{
    return (nb>>8) | (nb<<8);
}

static inline unsigned int htonl(unsigned int nb) {
       return ((nb>>24)&0xff)      |
              ((nb<<8)&0xff0000)   |
              ((nb>>8)&0xff00)     |
              ((nb<<24)&0xff000000);
}

static inline unsigned short ntohs(unsigned short nb)
{
    return (nb>>8) | (nb<<8);
}

static inline unsigned int ntohl(unsigned int nb) {
       return ((nb>>24)&0xff)      |
              ((nb<<8)&0xff0000)   |
              ((nb>>8)&0xff00)     |
              ((nb<<24)&0xff000000);
}


in_addr_t	inet_addr(const char *cp);

#endif
