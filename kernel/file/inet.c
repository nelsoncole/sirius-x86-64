#include <string.h>
#include <stdlib.h>
#include <inet.h>

in_addr_t	inet_addr(const char *cp){

    unsigned int ip_address;
    unsigned char *src = (unsigned char*)&ip_address;

    src[0] = 127;
    src[1] = 0;
    src[2] = 0;
    src[3] = 1;

    /*char *p = (char*)cp;
    
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
        src[3] = 0;*/

    return ip_address;
}
