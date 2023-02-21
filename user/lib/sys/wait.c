#include <sys/wait.h>
#include <stdio.h>

pid_t wait(int *iStatus){

    unsigned pid = 0;
    // syscall wait state
    __asm__ __volatile__("int $0x72"::"d"(30), "D"(iStatus));
    while(1){
        // syscall wait
        __asm__ __volatile__("int $0x72":"=a"(pid):"d"(31));
        if(pid)break;

        //wait...
    }
    return pid;
}