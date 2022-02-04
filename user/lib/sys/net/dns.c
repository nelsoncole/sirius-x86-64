#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#include "dns.h"


struct ANSWER
{
    unsigned short  name;
    unsigned short  type;
    unsigned short  class;
    unsigned int    ttl;
    unsigned short  data_len;
    unsigned char   address[4];

}__attribute__ ((packed));


char dns_servers[10][128] ={/*OpenDNS*/"208.67.222.222", "208.67.222.220", /*UNITEL DNS*/"10.172.224.10",
/*VM DNS*/ "10.0.2.3",/*Google DNS*/ "8.8.8.8", "8.8.4.4"};

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

char* get_ip_from_name(char *addr, const char *name , int query_type){

    int name_size = strlen(name);
    int dns_size = sizeof(struct DNS_HEADER) + name_size + 6;
    struct DNS_HEADER *dns = (struct DNS_HEADER*) malloc(dns_size + 0x1000);

    dns->id = htons(0x1234);
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
    dest.sin_addr.s_addr = inet_addr(dns_servers[2]); //dns ip
    dest.sin_port = htons(53);

    int socketid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketid < 0) {
        printf("Can't create socket!\n");
        return 0;
    }

    if( bind( socketid, (struct sockaddr*)&src, sizeof(struct sockaddr_in)) ) {
        printf("Can't bind socket!\n");
        return 0;
    }


    if( sendto(socketid, dns, dns_size, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0 ){
        printf("sendto failed\n"); 
        return 0;  
    }


    memset(dns, 0, sizeof(struct DNS_HEADER));

    socklen_t len = sizeof(dest);
    int count = recvfrom(socketid, dns, 0x1000, 0, (struct sockaddr*)&dest, &len);
    if( count < 1 ){
        printf("recvfrom failed\n");
        return 0;
    }

    int question_size = name_size + 6;
    if( htons(dns->answer_rr)&0x3 ) {
        //struct ANSWER *answer = (struct ANSWER*)((unsigned long)dns + sizeof(struct DNS_HEADER) + question_size);
        struct ANSWER *answer = (struct ANSWER*)((unsigned long)dns + (count-sizeof(struct ANSWER)));
        //printf("IP %d.%d.%d.%d\n", answer->address[0],answer->address[1],answer->address[2],answer->address[3]);


        sprintf(addr, "%d.%d.%d.%d", (int)answer->address[0],
        (int)answer->address[1],(int)answer->address[2],(int)answer->address[3]);
     
    }
    

    shutdown(socketid, 0);

    return addr;
}
