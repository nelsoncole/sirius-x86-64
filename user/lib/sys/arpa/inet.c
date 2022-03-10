#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

in_addr_t	inet_addr(const char *cp){

    unsigned int ip_address;
    unsigned char *src = (unsigned char*)&ip_address;

    char *p = (char*)cp;

    char *tmp = strtok(p,". ");
    if(tmp)
	    src[0]= strtoul (tmp,NULL, 10);
    else 
        src[0] = 0;

    tmp = strtok('\0', ". ");
    if(tmp)
	    src[1]= strtoul (tmp,NULL, 10);
    else 
        src[1] = 0;

    tmp = strtok('\0', ". ");
    if(tmp)
	    src[2]= strtoul (tmp,NULL, 10);
    else 
        src[2] = 0;

    tmp = strtok('\0', ". ");
    if(tmp)
	    src[3]= strtoul (tmp,NULL, 10);
    else 
        src[3] = 0;

    return ip_address;
}


in_addr_t	inet_lnaof(struct in_addr in){
    return 0;
}


struct in_addr	inet_makeaddr(in_addr_t net, in_addr_t lna){

    struct in_addr nul;
    memset(&nul, 0, sizeof(struct in_addr));
    return nul;
}


in_addr_t	inet_netof(struct in_addr in){
    return 0;
}


in_addr_t	inet_network(const char *cp){
    return 0;
}

char	*inet_ntoa(struct in_addr in){
    return 0;
}
