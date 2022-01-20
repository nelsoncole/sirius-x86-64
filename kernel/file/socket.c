#include <string.h>
#include <stdlib.h>

#include <socket.h>
#include <ethernet.h>



#define SOCKET_BUF_SIZE 8192
#define ROW_NUM 32

int socket_next_id;

struct socket_receive_row sockt_receive_row[ROW_NUM];
int row_cur;

static unsigned short port;
struct socket *current_saddr, *saddr_ready_queue;

int init_socket(int domain, int type, int protocol){
    port = 2000;
    current_saddr = saddr_ready_queue = 0;
    socket_next_id = 1;

    memset(sockt_receive_row, 0, sizeof(struct socket_receive_row)*ROW_NUM);
    row_cur = 0;

    unsigned long mem = 0;
    // 8KiB*ROW_NUM
    alloc_pages(0, ROW_NUM*2, (unsigned long*)&mem);

    for(int i=0; i < ROW_NUM; i++){
        sockt_receive_row[i].buffer = (unsigned char*)mem;
        mem += 0x2000;
    }
    

    if(domain != AF_LOCAL && domain != AF_INET) return -1;
    if(type != SOCK_STREAM && type != SOCK_DGRAM) return -1;


    struct socket *saddr = (struct socket*) malloc(sizeof(struct socket));
    current_saddr = saddr_ready_queue = saddr;

    memset(saddr, 0, sizeof(struct socket));

    saddr->id = socket_next_id++;
    saddr->domain = domain;
    saddr->type = type;
    saddr->protocol = protocol;

    saddr->buf1 = (unsigned long) malloc(SOCKET_BUF_SIZE);
    saddr->buf2 = (unsigned long) malloc(SOCKET_BUF_SIZE);

    saddr->client = 0;

    saddr->next = 0;

    return saddr->id;
}

int socket(int domain, int type, int protocol){
    
    if(domain != AF_LOCAL && domain != AF_INET) return -1;
    if(type != SOCK_STREAM && type != SOCK_DGRAM) return -1;


    struct socket *saddr = (struct socket*) malloc(sizeof(struct socket));
    memset(saddr, 0, sizeof(struct socket));

    saddr->id = socket_next_id++;
    saddr->domain = domain;
    saddr->type = type;
    saddr->protocol = protocol;

    saddr->buf1 = (unsigned long) malloc(SOCKET_BUF_SIZE);
    saddr->buf2 = (unsigned long) malloc(SOCKET_BUF_SIZE);
   
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


void socket_server_transmit(){
    char *buf;
    unsigned len;
    unsigned int dest_ip, src_ip;
    unsigned short dest_port, src_port;
    unsigned int seq;
    unsigned int ack;
    unsigned char protocol_flags;

    if(current_saddr->flags&0x80){
        if(current_saddr->flags&0x1){
            if(current_saddr->domain == AF_INET){
                src_ip = current_saddr->src_ip;
                dest_ip = current_saddr->dest_ip;
                src_port = htons(current_saddr->src_port);
                dest_port = htons(current_saddr->dest_port);
                seq = current_saddr->seq;
                ack = current_saddr->ack;
                protocol_flags = current_saddr->protocol_flags;
                buf = (char*)current_saddr->buf2;
                len = current_saddr->length2;

                if(!src_ip) {
                    fillIP((unsigned char*)&current_saddr->src_ip, our_ip);
                    src_ip = current_saddr->src_ip;
                }

                if(!src_port) {
                    if(port < 2000) port = 2000;
                    current_saddr->src_port = htons(port++);
                    src_port = htons(current_saddr->src_port);
                }

                switch(current_saddr->type){
                    case SOCK_DGRAM:
                        udp_send(src_ip, dest_ip, src_port, dest_port, buf, len);
                        break;
                    case SOCK_STREAM:
                        tcp_send(src_ip, dest_ip, src_port, dest_port, seq, ack, protocol_flags, buf, len);
                        break;
                }
            }
            // clean
            current_saddr->flags &=~0x1;
        }

    }

    // Obter a próxima tarefa a ser executada.
	current_saddr 	= current_saddr->next;
    // Se caímos no final da lista vinculada, 
    // comece novamente do início.
    if (!current_saddr) {
    	current_saddr = saddr_ready_queue;
    }
}

static void socket_save_row(int protocol, unsigned int src_ip, unsigned int dest_ip, unsigned short src_port, unsigned short dest_port,
    const void *buffer, unsigned length, unsigned int seq, unsigned int ack, unsigned char flags){

    int old_cur = row_cur;
    row_cur = (row_cur + 1) % ROW_NUM; // next

    if(0){} // dados perdidos

    sockt_receive_row[old_cur].protocol = protocol;
    sockt_receive_row[old_cur].src_port = src_port;
    sockt_receive_row[old_cur].src_ip = src_ip;

    sockt_receive_row[old_cur].dest_port = dest_port;
    sockt_receive_row[old_cur].dest_ip = dest_ip;

    // For TCP
    sockt_receive_row[old_cur].seq = seq;
    sockt_receive_row[old_cur].ack = ack;
    sockt_receive_row[old_cur].flags = flags;
    sockt_receive_row[old_cur].win = 0;

    if(length) {
        memcpy(sockt_receive_row[old_cur].buffer, buffer, length);
    }
    sockt_receive_row[old_cur].length = length;

    sockt_receive_row[old_cur].status = 1;

}

void socket_execute_row(){
    for(int i = 0 ; i < ROW_NUM; i++){

        if((sockt_receive_row[i].status & 0x1))
        {
            
            if(!socket_server_receive(1, sockt_receive_row[i].protocol, sockt_receive_row[i].src_ip,
                sockt_receive_row[i].dest_ip, sockt_receive_row[i].src_port, sockt_receive_row[i].dest_port,
                sockt_receive_row[i].buffer, sockt_receive_row[i].length, sockt_receive_row[i].seq,
                sockt_receive_row[i].ack, sockt_receive_row[i].flags) ){

                sockt_receive_row[i].status = 0;
            }
        }
    }

}

int socket_server_receive(int origem, int protocol, unsigned int src_ip, unsigned int dest_ip, unsigned short src_port, unsigned short dest_port,
    const void *buffer, unsigned length, unsigned int seq, unsigned int ack, unsigned char flags){

    char *dest_buf;

    struct socket *saddr = saddr_ready_queue;
    while(saddr){
        if(saddr->flags&0x80){
            if(saddr->domain == AF_INET){
                if(saddr->src_port == dest_port){
                
                    if(saddr->flags&2){
                        // TODO despachar o pacote para fila
                        if(!origem){
                            socket_save_row(protocol, src_ip, dest_ip, src_port, dest_port, buffer, length, seq, ack, flags);
                        }
                        return 1;
                    }

                    dest_buf = (char*)saddr->buf1;
                    saddr->length1 = length;
                    saddr->dest_ip = src_ip;
                    saddr->dest_port = src_port;

                    if(IP_PROTOCOL_TCP == protocol ){
                        saddr->seq = htonl(seq);
                        saddr->ack = htonl(ack);
                        saddr->protocol_flags = flags;
                    }
                    
                    if(length) {
                        memcpy(dest_buf, buffer , length);
                    }
                    saddr->flags |= 2;
                    break;
                }
            }
        }

        saddr = saddr->next;
    }

    return 0;
}
