#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#include "dns.h"
unsigned char dns_buf[16];


void changeto_dns_name_format(unsigned char *name, int size) 
{
    unsigned char *cp = name+1;
    int position = 0;
    int n = 0;
    for(int i = 0; i < size ; i++) 
    {
        if(cp[i] == '\0')
            return;
        
        if(cp[i] == '.'){
            name[position] = n;
            n = 0;
            position = i + 1;
        }else
            n++;
    }

    name[position] = n;
    name[size+1] ='\0';
}

unsigned char* get_ip_from_name(const char *name , int query_type){

    memset(dns_buf, 0, 16);
    int name_size = strlen(name);
    int dns_size = sizeof(struct DNS_HEADER) + name_size + 6;
    struct DNS_HEADER *dns = (struct DNS_HEADER*) malloc(dns_size + 0x1000);

    dns->id = htons(0x1);
    dns->flags = htons(0x0100);
    dns->q_count = htons(1); //we have only 1 question
    dns->answer_rr = htons(0);
    dns->authority_rr = htons(0);
    dns->aditional_rr = htons(0);

    unsigned char *query = (unsigned char *)((unsigned long)dns + sizeof(struct DNS_HEADER));
    memcpy(query+1, name, name_size);

    changeto_dns_name_format(query, name_size);
    query += name_size + 2;

    *(unsigned short*)(query) = htons(query_type);
    query += 2;
    *(unsigned short*)(query) = htons(1);


    //---------------
    struct sockaddr_in src;
    struct sockaddr_in dest;

    src.sin_family = AF_INET;
    src.sin_addr.s_addr = htonl(INADDR_ANY);
    src.sin_port = htons(0);

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = /*inet_addr("10.0.2.3");*/inet_addr("208.67.222.222"); //dns ip
    dest.sin_port = htons(53);

    int socketid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketid < 0) {
        printf("Can't create socket!\n");
        return 0;
    }

    if( bind( socketid, (struct sockaddr*)&dest, sizeof(struct sockaddr_in)) ) {
        printf("Can't bind socket!\n");
        return 0;
    }

    printf("Sending Packet...");
    if( sendto(socketid, dns, dns_size, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0 ){
        perror("sendto failed");   
    }
    printf("Done\n");

    socklen_t len = sizeof(dest);
    printf("\nReceiving answer...");
    if( recvfrom(socketid, dns, 0x1000, 0, (struct sockaddr*)&dest, &len) < 0 ){
        perror("recvfrom failed\n");
    }

    printf("Done\n");

    shutdown(socketid, 0);

    return dns_buf;
}
