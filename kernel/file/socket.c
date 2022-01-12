#include <socket.h>


int socket_next_id;


struct socket *current_saddr, *saddr_ready_queue;

int init_socket(int domain, int type, int protocol){

    current_saddr = saddr_ready_queue = 0;
    socket_next_id = 0;
    
    if(domain != AF_LOCAL && domain != AF_INET) return -1;
    if(type != SOCK_STREAM && type != SOCK_DGRAM) return -1;


    struct socket *saddr = (struct socket*) malloc(sizeof(struct socket));
    current_saddr = saddr_ready_queue = saddr;

    saddr->id = socket_next_id++;
    saddr->domain = domain;
    saddr->type = type;
    saddr->protocol = protocol;
    saddr->address_port = 0;
    saddr->address_ip = 0;

    saddr->flags = 0;
    saddr->length = 0;
    saddr->buf = 0;

    saddr->num_client = 0;
    saddr->client = 0;

    saddr->next = 0;

    return saddr->id;
}

int socket(int domain, int type, int protocol){
    
    if(domain != AF_LOCAL && domain != AF_INET) return -1;
    if(type != SOCK_STREAM && type != SOCK_DGRAM) return -1;


    struct socket *saddr = (struct socket*) malloc(sizeof(struct socket));

    saddr->id = socket_next_id++;
    saddr->domain = domain;
    saddr->type = type;
    saddr->protocol = protocol;
    saddr->address_port = 0;
    saddr->address_ip = 0;

    saddr->flags = 0;
    saddr->length = 0;
    saddr->buf = 0;

    saddr->num_client = 0;
    saddr->client = 0;

    saddr->next = 0;

    // Adicionar novo elemento, no final da lista
    // tmp aponta para inicio da lista
    struct socket *tmp = saddr_ready_queue;
    while (tmp->next) {
    	
    	tmp = tmp->next;
    }
    	
    tmp->next = saddr;

    return saddr->id;
}
