#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern char* get_ip_from_name(char *addr, const char *name , int query_type);
int main(int argc, char **argv) {

    if(argc < 2) {
        printf("No IP...\n");
        return 0;
    }

    char ip[128];
   
    /*if(!get_ip_from_name(ip, argv[1], 1)){
        printf("DNS No IP...\n");
        return 0;
    }*/

    struct sockaddr_in sera;
    struct sockaddr_in serb;
    sera.sin_family        = AF_INET;
    sera.sin_addr.s_addr = htonl(INADDR_ANY);
    sera.sin_port  = htons(0);

    serb.sin_family        = AF_INET;
    serb.sin_addr.s_addr = inet_addr(/*(const char*)ip*/argv[1]);
    serb.sin_port  = htons(20001);

    int socketid = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketid < 0) {
        printf("Cannot create socket!\n");
        return 1;
    }
    int r = bind( socketid, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        return 1;
    }
    char message[256];
    while(1) {
        printf("> ");
        fgets (message,256,stdin);
        ssize_t count = sendto(socketid, message, strlen(message), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));
        memset(message, 0, 256);
        printf("< ");
        socklen_t len = sizeof(serb);
        count = recvfrom(socketid, message, 256, 0, (struct sockaddr*)&serb, &len);    
        printf("%s\n",message);
    }
    shutdown(socketid, 0);
	return 0;
}
