
#ifndef __IN_H__
#define __IN_H__

#include <sys/socket.h>

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

#endif

