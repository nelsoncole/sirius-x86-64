#include <pthread.h>
#include <stdlib.h>


int pthread_create(pthread_t *pd, const pthread_attr_t *attr,
          void *(*start_routine)(void *), void *arg){

    unsigned long rax = 0;
    unsigned long stack = (unsigned long)malloc(8192); // 8KiB
    stack += 8192-1;
    unsigned long start = (unsigned long)start_routine;
    __asm__ __volatile__("int $0x72;":"=a"(rax):"d"(17),"D"(start),"S"(stack));

    pthread_t tmp = (pthread_t)malloc(sizeof(pthread_t));

    tmp->p = (void*)rax;

    *pd = tmp; 

    return 0;
}


void pthread_exit(void *retval){
    while(1){}
}


int pthread_join(pthread_t pd, void **retval){
    unsigned long rax = 0;
    unsigned long start = (unsigned long)pd->p;
    __asm__ __volatile__("int $0x72;":"=a"(rax):"d"(18),"D"(start));
    return 0;
}
