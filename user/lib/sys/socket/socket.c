#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

    unsigned char   flags;
    unsigned int    length1;
    unsigned int    length2;
    unsigned long   buf1;
    unsigned long   buf2;

    unsigned char   num_client;   
    struct socket *client;

    struct socket *next;
}__attribute__((packed));



void *socket_address(int socket){
    unsigned long addr = 0;
    __asm__ __volatile__("int $0x72;":"=a"(addr):"d"(16));

    struct socket *saddr_ready_queue = (struct socket *)addr;

    struct socket *tmp = saddr_ready_queue;
    while (tmp) {

        if(tmp->id == socket)
            break;

    	tmp = tmp->next;
    }
    
    return (void*)tmp;
}

int bind(int socket, const struct sockaddr *address,
             socklen_t address_len){

    if(!address_len)
        return -1;
    
    struct socket *fd = (struct socket *)socket_address(socket);
    struct sockaddr_in saddr;

    memcpy((char*)&saddr, address, address_len);

    if(saddr.sin_family != fd->domain)
        return -1;

    fd->src_port = saddr.sin_port;
    fd->src_ip = saddr.sin_addr.s_addr;


    // permitido
    fd->flags |= 0x80;

    return 0;
}
ssize_t recv(int socket, void *buffer, size_t length, int flags){
    return 0;
}

ssize_t recvfrom(int socket, void *buffer, size_t length,
             int flags, struct sockaddr *address, socklen_t *address_len){

    *address_len = 0;
    if(!length)
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

    if(fd->length1 < length)
        length = fd->length1;

    memcpy(buffer, (char*)fd->buf1 ,length);


    // clean    
    fd->flags &=~0x2;
    return length;
}

ssize_t send(int socket, const void *message, size_t length, int flags){
    return 0;
}

ssize_t sendto(int socket, const void *message, size_t length, int flags,
             const struct sockaddr *dest_addr, socklen_t dest_len){


    if(!length)
        return 0;

    if(!message)
        return 0;

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return 0;

    if(!(fd->flags&0x80))
        return 0;


    struct sockaddr_in saddr;
    memcpy((char*)&saddr, dest_addr, dest_len);


    fd->dest_port = saddr.sin_port;
    fd->dest_ip = saddr.sin_addr.s_addr;

    fd->length2 = length;

    memcpy((char*)fd->buf2, message, length);


    fd->flags |= 0x1; // send

    // polling
    while(fd->flags&0x1);


    return length;
}

int shutdown(int socket, int how){

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return -1;

    fd->flags = 0;
    
    return 0;
}

int socket(int domain, int type, int protocol){
    int r = 0;
    __asm__ __volatile__("int $0x72;":"=a"(r):"d"(15),"c"(protocol),"S"(type),"D"(domain));
    return r;
}
