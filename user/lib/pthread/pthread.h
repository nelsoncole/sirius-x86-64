#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include <sys/types.h>

struct	pthread{
    void * p;
    unsigned int x;
};

struct	pthread_attr{
    unsigned int x;
};

typedef void *pthread_addr_t;
typedef struct	pthread         *pthread_t;
typedef struct	pthread_attr    *pthread_attr_t;

int pthread_create(pthread_t *, const pthread_attr_t *,
          void *(*)(void *), void *);

void pthread_exit(void *);

int pthread_join(pthread_t, void **);


#endif
