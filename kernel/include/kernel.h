#ifndef __KERNEL_H
#define __KERNEL_H

#include <thread.h>

#define COUNT_ARGV 32

unsigned long exectve_child(int argc, char **argv, char *pwd, void *bf, THREAD *thread );
unsigned long exectve(int argc, char **argv, char *pwd, void *bf);
void server(unsigned long entry_pointer_info);

void kernel_panic(const char *fmt,...);



#endif
