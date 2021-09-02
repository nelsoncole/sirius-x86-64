#ifndef __KERNEL_H
#define __KERNEL_H

#define COUNT_ARGV 32

unsigned long exectve_child(int argc, char **argv, char *pwd, FILE *fp, THREAD *thread );
unsigned long exectve(int argc, char **argv, char *pwd, FILE *fp);
void server(unsigned long entry_pointer_info);



#endif
