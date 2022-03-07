#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/communication.h>

int communication(struct communication *commun, struct communication *ret){
    if(commun == 0 || ret == 0 ) return 1;
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    sera.sin_family         = AF_LOCAL;
    sera.sin_addr.s_addr    = htonl(INADDR_ANY);
    sera.sin_port           = htons(0);

    serb.sin_family         = AF_LOCAL;
    serb.sin_addr.s_addr    = inet_addr("127.0.0.1");
    serb.sin_port           = htons(1024);

    if(sd < 0) {
        printf("Cannot create socket!\n");
        while(1){}
    }
    int r = bind( sd, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        while(1){}
    }

    ssize_t count = sendto(sd, commun, sizeof(struct communication), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));
    socklen_t len;
    ssize_t to = recvfrom(sd, ret, sizeof(struct communication), 0, (struct sockaddr*)&serb, &len);

    shutdown(sd, 0);
    return 0;
}

