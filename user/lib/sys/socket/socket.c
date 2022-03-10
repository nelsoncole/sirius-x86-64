#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>

#define TCP_FIN     (1 << 0)
#define TCP_SYN     (1 << 1)
#define TCP_RST     (1 << 2)
#define TCP_PSH     (1 << 3)
#define TCP_ACK     (1 << 4)
#define TCP_URG     (1 << 5)

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
    // receive
    unsigned short  recv_dest_port;
    unsigned int    recv_dest_ip;

    // For TCP
    unsigned char   protocol_flags;
    unsigned short  src_win;
    unsigned short  dst_win;

    //
    unsigned char   connect;

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

int accept(int socket, struct sockaddr *address,
             socklen_t *address_len){
    return -1;
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

int connect(int socket, const struct sockaddr *address,
             socklen_t address_len){
    if(!address_len || !address)
        return -1;

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return -1;

    struct sockaddr_in saddr;
    memcpy((char*)&saddr, address, address_len);

    // espera terminar
    while(fd->flags&1);

    // Configurar o Server
    fd->dest_port = saddr.sin_port;
    fd->dest_ip = saddr.sin_addr.s_addr;

    if(saddr.sin_family != AF_INET){
        printf("connected family %x protocol %d\n", saddr.sin_family, fd->protocol);
        return -1;
    }

    // permitido
    fd->flags |= 0x80;

    // TCP INIT
    fd->protocol_flags = TCP_SYN;
    fd->length2 = 0;

    // send packet
    fd->flags |= 0x1;
    // polling
    while(fd->flags&0x1);

    // wait packet
    while(!(fd->flags&0x2));
    // clean    
    fd->flags &=~0x2;
    
    if(0) {
        // verificar erro
        return -1;
    }

    fd->connect = 1;

    return 0;
}

int listen(int socket, int backlog){
    return 0;
}

ssize_t recv(int socket, void *buffer, size_t length, int flags){
    if(!length || !buffer)
        return 0;
    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return 0;
    if(!(fd->flags&0x80))
        return 0;

    // espera terminar
    while(fd->flags&1){}

    // pooling
    while(!(fd->flags&0x2)){}

    if(fd->protocol_flags != (TCP_PSH | TCP_ACK )) {
        // clean    
        fd->flags &=~0x2;
        return 0;
    }
    
    if(fd->length1 < length){
        length = fd->length1;
    }
    
    memcpy(buffer, (char*)fd->buf1 ,length);
    // clean    
    fd->flags &=~0x2;

    return length;
}

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
    while((fd->flags&0x2) == 0){};

    *address_len = sizeof(struct sockaddr_in);

    struct sockaddr_in saddr;
    saddr.sin_family = fd->domain;
    saddr.sin_port = fd->recv_dest_port;
    saddr.sin_addr.s_addr = fd->recv_dest_ip;
    memcpy(address, (char*)&saddr, *address_len);

    if(fd->length1 < length){
        length = fd->length1;
    }

    memcpy(buffer, (char*)fd->buf1 ,length);

    // clean    
    fd->flags &=~0x2;
    return length;
}

ssize_t send(int socket, const void *message, size_t length, int flags){
    if(!message && length)
        return 0;

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return 0;

    if(!(fd->flags&0x80))
        return 0;


    if(!(fd->dest_ip | fd->dest_port))
        return 0;

    // espera terminar
    while(fd->flags&1);

    fd->length2 = length;

    if(fd->dst_win < length){
        // TODO tem que rever esse tipo de situacao
    }

    if(length){
        memcpy((char*)fd->buf2, message, length);
    }
    

    fd->protocol_flags = TCP_PSH | TCP_ACK;
    // send packet
    fd->flags |= 0x1;
    // polling
    while(fd->flags&0x1){}
   
    return length;
}

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
    while(fd->flags&1);

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

int shutdown(int socket, int how){

    struct socket *fd = (struct socket *)socket_address(socket);
    if(!fd)
        return -1;

    if(!(fd->flags&0x80))
        return -1;

    // espera terminar
    while(fd->flags&1);


    if(fd->type == SOCK_STREAM && (fd->connect&0x1)) {
        // TCP FIN + ACK
        fd->protocol_flags = TCP_FIN | TCP_ACK;
        fd->length2 = 0;
        // send packet
        fd->flags |= 0x1;
        // polling
        while(fd->flags&0x1){}

        fd->connect = 0;
    }


    fd->flags = 0;
    
    return 0;
}

int socket(int domain, int type, int protocol){
    int r = 0;
    __asm__ __volatile__("int $0x72;":"=a"(r):"d"(15),"c"(protocol),"S"(type),"D"(domain));
    return r;
}
