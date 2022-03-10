#include <stdio.h>
#include <stdlib.h>
#include <socket.h>
#include <inet.h>
#include <string.h>
#include <thread.h>
#include <kernel.h>
#include <acpi.h>
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
    unsigned char message[1024-24];
};

unsigned long memory = 0;
extern int format_argv(int *argc, char **argv, char *src);
extern unsigned long thread_id;
extern void foco(unsigned long pid, unsigned long id);
extern int pipe_write ( void *buf, FILE *fp);
extern void exit(int rc, THREAD *thr);
int commun_switch(struct communication *commun, struct communication *ret)
{
    short pipe[8] = {0,0,0,0,0,0,0,0};
	pipe[0] = 0x1001;
    THREAD *thread;

    char *buf;
    char *pwd;
    unsigned long *addr;
    unsigned long pid;
    char *option;
    int argc;
    char **argv;
    switch(commun->type){
        case COMMUN_TYPE_REBOOT:
            strcpy((char*)ret->message,"reboot\n" );
            reboot();
            return 0;
        case COMMUN_TYPE_POWEROFF:
            strcpy((char*)ret->message,"poweroff\n" );
            poweroff();
            return 0;
        case COMMUN_TYPE_EXIT:
            strcpy((char*)ret->message,"exit\n" );
            thread = get_thread(commun->pid);
            exit(0, thread);
            return 0;
        case COMMUN_TYPE_FOCO:
            strcpy((char*)ret->message,"foco\n" );
            foco(commun->pid,commun->apid);
            return 0;
        case COMMUN_TYPE_EXEC:
            strcpy((char*)ret->message,"exective\n" );
            addr = (unsigned long*)((unsigned long)&commun->message);
            cli();
			buf = (char *) addr[0];
			pwd = (char *)&addr[1];
            thread_id = commun->apid; // usado para registro da thread ID 
			exectve(0, 0, pwd, buf);
            thread_id = 0;
			sti();
            return 0;
        case COMMUN_TYPE_EXEC_CHILD:
            strcpy((char*)ret->message,"exective child\n" );
            addr = (unsigned long*)((unsigned long)&commun->message);
            cli();
			buf = (char *) addr[0];
			pid = commun->pid;
            option = (char *)&addr[1];
			pwd = option;
            argc = 0;
            argv = (char**)memory;
            int i = 0;
            for(i=1000; i > 0; i--){ 
                if(*option++ == '\0')break;
            }
    
            if(i <= 0) {
                argv = (char**)0;
            }else {
                format_argv(&argc, argv, option);
            }
		
			if(!exectve_child(argc, argv, pwd, buf, get_thread(pid)))
            {	// error
				thread = get_thread(pid);
				pipe_write ( pipe, thread->pipe);
                
			}
			sti();
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

    memory = (unsigned long) malloc(0x1000);
    struct communication *commun = (struct communication*) malloc(sizeof(struct communication));
    struct communication *commun2 = (struct communication*)malloc(sizeof(struct communication));
    char *message = "sirius operating system\n\0";
    while(1){
        socklen_t len;
        ssize_t to = recvfrom(sd, commun, sizeof(struct communication), 0, (struct sockaddr*)&serb, &len);

        commun_switch(commun, commun2);
        ssize_t count = sendto(sd, commun2, sizeof(struct communication), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));
        
    }

}
