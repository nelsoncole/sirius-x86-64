#include <stdio.h>
#include <socket.h>
#include <inet.h>
#include <string.h>
#include <thread.h>
#include <io.h>

#define COMMUN_TYPE 0x1000
#define COMMUN_TYPE_REBOOT (COMMUN_TYPE + 1)
#define COMMUN_TYPE_POWEROFF (COMMUN_TYPE_REBOOT + 1)
#define COMMUN_TYPE_EXIT (COMMUN_TYPE_POWEROFF + 1)
#define COMMUN_TYPE_FOCO (COMMUN_TYPE_EXIT + 1)
#define COMMUN_TYPE_EXEC (COMMUN_TYPE_FOCO + 1)
#define COMMUN_TYPE_EXEC_CHILD (COMMUN_TYPE_EXEC + 1)

// communication between processes
struct communication{
    unsigned long type;
    unsigned long pid;
    unsigned long apid;
    unsigned char message[512-24];
};

extern void foco(unsigned long pid, unsigned long id);
int commun_switch(struct communication *commun, struct communication *ret)
{

    THREAD *thread;

    switch(commun->type){
        case COMMUN_TYPE_REBOOT:
            strcpy(&ret->message,"reboot\n" );
            reboot();
            return 0;
        case COMMUN_TYPE_POWEROFF:
            strcpy(&ret->message,"poweroff\n" );
            poweroff();
            return 0;
        case COMMUN_TYPE_EXIT:
            strcpy(&ret->message,"exit\n" );
            thread = get_thread(commun->pid);
			exit(0, thread);
            return 0;
        case COMMUN_TYPE_FOCO:
            strcpy(&ret->message,"foco\n" );
            foco(commun->pid,commun->apid);
            return 0;
        case COMMUN_TYPE_EXEC:
            strcpy(&ret->message,"exective\n" );
            return 0;
        case COMMUN_TYPE_EXEC_CHILD:
            strcpy(&ret->message,"exective child\n" );
            return 0;
    }

    return 0;
}

void kernel_thread_communication(){
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    sera.sin_family         = AF_LOCAL;
    sera.sin_addr.s_addr    = inet_addr("127.0.0.1");
    sera.sin_port           = htons(1024);

    serb.sin_family         = AF_LOCAL;
    serb.sin_addr.s_addr    = htonl(INADDR_ANY);
    serb.sin_port           = htons(0);

    if(sd < 0) {
        printf("Cannot create socket!\n");
        while(1){}
    }
    int r = bind( sd, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        while(1){}
    }

    struct communication commun;
    struct communication commun2;
    char *message = "sirius operating system\n\0";
    while(1){
        socklen_t len;
        ssize_t to = recvfrom(sd, &commun, sizeof(struct communication), 0, (struct sockaddr*)&serb, &len);
    
        commun_switch(&commun, &commun2);
        ssize_t count = sendto(sd, &commun2, sizeof(struct communication), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));

    }

}

