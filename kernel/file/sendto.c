#include <string.h>
#include <socket.h>
#include <inet.h>

#include <stdio.h>

ssize_t sendto(int socket, const void *message, size_t length, int flags,
             const struct sockaddr *dest_addr, socklen_t dest_len){

    if(!message && length )
        return 0;

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return 0;

    if(!(fd->flags&0x80))
        return 0;


    struct sockaddr_in saddr;
    memcpy((char*)&saddr, dest_addr, dest_len);

    // espera terminar
    while(fd->flags&0x1){};

    fd->dest_port = saddr.sin_port;
    fd->dest_ip = saddr.sin_addr.s_addr;

    fd->length2 = length;

    if(length){
        memcpy((char*)fd->buf2, message, length);
    }

    // send packet
    fd->flags |= 0x1;
    // polling
    while(fd->flags&0x1);

    return length;
}

