#include <sys/socket.h>

int bind(int socket, const struct sockaddr *address,
             socklen_t address_len){
    return 0;
}
ssize_t recv(int socket, void *buffer, size_t length, int flags){
    return 0;
}

ssize_t send(int socket, const void *message, size_t length, int flags){
    return 0;
}

int shutdown(int socket, int how){
    return 0;
}

int socket(int domain, int type, int protocol){
    int r = 0;
    __asm__ __volatile__("int $0x72;":"=a"(r):"d"(15),"c"(protocol),"S"(type),"D"(domain));
    return r;
}
