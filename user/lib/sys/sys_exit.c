#include <sys/communication.h>

void sys_exit(int rc){
    unsigned long pid = 0;
    // getpid()
    __asm__ __volatile__("int $0x72":"=a"(pid):"d"(1),"c"(0));
    struct communication commun;

    commun.type = COMMUN_TYPE_EXIT;
    commun.pid = pid;
    communication(&commun, &commun);

    while(1){}
}
