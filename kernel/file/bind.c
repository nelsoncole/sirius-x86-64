#include <string.h>
#include <socket.h>
#include <inet.h>
#include <ethernet.h>

extern unsigned char your_ip[4];
extern unsigned char local_ip[4];

int bind(int socket, const struct sockaddr *address,
             socklen_t address_len){

    if(!address_len)
        return -1;
    
    struct socket *fd = (struct socket *)socket_address(socket);
    struct sockaddr_in saddr;

    memcpy((char*)&saddr, address, address_len);

    if(saddr.sin_family != fd->domain)
        return -1;

    if ( !set_port(htons(saddr.sin_port)) ){
        // porta ocupada.
        return -1;
    }

    fd->src_port = saddr.sin_port;
    fd->src_ip = saddr.sin_addr.s_addr;

    if (!fd->src_ip){
        unsigned int ip = 0;
        if(fd->domain == AF_LOCAL)
            fillIP((unsigned char*)&ip, local_ip);
        else
            fillIP((unsigned char*)&ip, your_ip);
        fd->src_ip = ip;
    }

    if (!fd->src_port){
        fd->src_port = htons(get_port());
        if(!fd->src_port)return -1;
    }

    // permitido
    fd->flags |= 0x80;

    return 0;
}
