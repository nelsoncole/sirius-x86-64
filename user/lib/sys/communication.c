#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/communication.h>
#include <string.h>

int communication(struct communication *commun, struct communication *ret, int port){
    if(commun == 0 || ret == 0 ) return 1;
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    sera.sin_family         = AF_LOCAL;
    sera.sin_addr.s_addr    = htonl(INADDR_ANY);
    sera.sin_port           = htons(0);

    serb.sin_family         = AF_LOCAL;
    serb.sin_addr.s_addr    = inet_addr("127.0.0.1");
    serb.sin_port           = htons(port);

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

int con_init(struct con *con){

    memset(con, 0, sizeof(struct con));
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    unsigned short port = ether_getport();

    sera.sin_family         = AF_LOCAL;
    sera.sin_addr.s_addr    = htonl(INADDR_ANY);
    sera.sin_port           = htons(port);

    serb.sin_family         = AF_LOCAL;
    serb.sin_addr.s_addr    = inet_addr("127.0.0.1");
    serb.sin_port           = htons(1025);

    if(sd < 0) {
        printf("Cannot create socket!\n");
        return -1;
    }
    int r = bind( sd, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        return -1;
    }

    memcpy(&con->sera, &sera, sizeof(struct sockaddr_in));
    memcpy(&con->serb, &serb, sizeof(struct sockaddr_in));
    con->port = port;
    con->socket = sd;

    return 0;
}

ssize_t con_sendto(struct con *con){
    return sendto(con->socket, &con->req, sizeof(struct communication), 0, (struct sockaddr*)&con->serb, sizeof(struct sockaddr_in));
}

ssize_t con_recvfrom(struct con *con){
    socklen_t len;
    struct sockaddr_in serb;
    return recvfrom(con->socket, &con->ack, sizeof(struct communication), 0, (struct sockaddr*)&serb, &len);
}
