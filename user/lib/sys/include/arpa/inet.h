#ifndef __INET_H__
#define __INET_H__

#include <netinet/in.h>

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
in_addr_t	inet_lnaof(struct in_addr in);
struct in_addr	inet_makeaddr(in_addr_t net, in_addr_t lna);
in_addr_t	inet_netof(struct in_addr in);
in_addr_t	inet_network(const char *cp);
char	*inet_ntoa(struct in_addr in);

#endif
