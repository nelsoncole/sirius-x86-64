#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// NICK Nelson
// PING :36D9E40F
// PONG 36D9E40F
// USER Nelson 0 \* : Nelson Cole
// PRIVMSG #KernelDev : Olá Lucky
extern void term_color(int fg, int bg);
extern char* get_ip_from_name(char *addr, const char *name , int query_type);
char bf[4096];
int main(int argc, char **argv) {

    term_color(14, 2);
    printf(" Hello ");
    term_color(-1, -1);
    printf(" ");
    term_color(4, 15);
    printf(" World!\n");
    printf("\n");
    term_color(0, 15);
    for(int i=0; i < 40; i++){
        printf(" ");
        int i = 1000000;
        while(i-- > 0){}
    }
    printf("\n");
    return 0;
    if(argc < 2) {
        printf("No IP...\n");
        return 0;
    }

    if(argc < 3) {
        printf("Port Null...\n");
        return 0;
    }

    int dns = 0;
    if(argc < 4) {
        dns = 1;
    }

    int port = strtoul (argv[2],NULL, 10);
    char ip[128];
    if(!dns)
        strcpy(ip, argv[1]);
    else
    if(!get_ip_from_name(ip, argv[1], 1)){
        printf("DNS No IP...\n");
        return 0;
    }

    printf("IP: %s, Port: %d\n", ip, port);

    int client;
    struct sockaddr_in  saddr;

    saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //printf("Socket criado\n");
    
    if( connect(client, (struct sockaddr*)&saddr, sizeof(saddr)) ){
	    printf("Erro na concaxao\n");
	    shutdown(client, 1);
	    return (0);
    }
    //printf("Conetado ao servidor\n");

    int y = 0;
    while(1){
        //
        memset(bf, 0, 1024);
        printf("< ");
        int r = recv(client, bf, 1024, 0);
        printf("%s",bf);
    
        if(y >= 1 || dns == 0 ) {

            printf("> ");
            fgets(bf,1024,stdin);
            send(client, bf, strlen(bf),0);
        }

        y++;
    }
    shutdown(client, 0);
	return 0;
}



