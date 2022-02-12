#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

extern char* get_ip_from_name(char *addr, const char *name , int query_type);
int client;
char *buf1, *buf2;
pthread_t thread;

void *f1(){

    while(1){
        memset(buf1,0,0x1000);
        int len = recv(client, buf1, 0x1000, 0);
        if(len){
            printf("%s", buf1);
        }
    }

    pthread_exit(0);
}

int main(int argc, char **argv){

    if(argc < 3) {
        printf("Telnet: requer argumento\n");
        return 0;
    }

    int port = strtoul (argv[2],NULL, 10);
    char ip[128];
    if(!get_ip_from_name(ip, argv[1], 1)){
        printf("Telnet: dns no IP address...\n");
        return 0;
    }

    printf("Telnet: IP: %s, Port: %d\n", ip, port);

    struct sockaddr_in  saddr;

    saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(!client){
        printf("Telnet: erro na criaco do socket()\n");
        return (0);
    }

    if( connect(client, (struct sockaddr*)&saddr, sizeof(saddr)) ){
	    printf("Telnet: erro na concaxao\n");
	    shutdown(client, 1);
	    return (0);
    }

    buf2 =(char*)malloc(0x1000); 
    buf1 =(char*)malloc(0x1000);
    
    int i1;
    pthread_create(&thread,NULL,f1,&i1);
    pthread_join(thread,NULL);

    while(1){

        memset(buf2,0,0x1000);
        fgets(buf2,1024, stdin);
        char *a = buf2 + strlen(buf2) -1;
        send(client, buf2, strlen(buf2),0);
    }

    return 0;
}
