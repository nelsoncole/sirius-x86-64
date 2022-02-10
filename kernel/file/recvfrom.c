#include <string.h>
#include <socket.h>
#include <inet.h>

ssize_t recvfrom(int socket, void *buffer, size_t length,
             int flags, struct sockaddr *address, socklen_t *address_len){

    *address_len = 0;
    if(!length || !address || !buffer)
        return 0;

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return 0;

    
    if(!(fd->flags&0x80))
        return 0;

    // pooling
    while(!(fd->flags&0x2));

    *address_len = sizeof(struct sockaddr_in);

    struct sockaddr_in saddr;
    saddr.sin_family = fd->domain;
    saddr.sin_port = fd->dest_port;
    saddr.sin_addr.s_addr = fd->dest_ip;
    memcpy(address, (char*)&saddr, *address_len);

    if(fd->length1 < length){
        length = fd->length1;
    }

    memcpy(buffer, (char*)fd->buf1 ,length);


    // clean    
    fd->flags &=~0x2;
    return length;
}
