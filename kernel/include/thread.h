#ifndef __THREAD_H
#define __THREAD_H

#include <stdio.h>
#include <data.h>

#define THREAD_ZUMBI 1

typedef struct _THREAD {
	// Registradores geral
	unsigned long rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp, rip;
	unsigned long r8, r9, r10, r11, r12, r13, r14, r15;

	// Registradores  segmento
    unsigned long cs, ds, es, fs, gs, ss;
	
	// Outros registradores 
	unsigned long rflag, cr3;

	// Registradores null
	unsigned long rsp0, pdpte;
	
	// ID de tarefa  (conhecido como PID)
    unsigned long pid, prv, status;

	// Stream default
	FILE *stdin, *stdout, *stderr, *pipe;
	
	mouse_t *mouse;
	
	// windows
	unsigned long window, id;
	
	
	// outros
	unsigned long pml4e_addr, pdpte_addr, pde_addr, pte_addr, frame_size;
	unsigned long alloc_addr;
	
	
	char fxsave[512]__attribute__((aligned(16)));
	
	
	// linker do próximo     
    struct _THREAD *next;
    struct _THREAD *tail;

}__attribute__((packed)) THREAD;


extern THREAD	*current_thread, *thread_ready_queue;
extern THREAD	*current_thread1, *current_thread2;

unsigned long thread_setup();
THREAD *create_thread( void (*main)(), unsigned long rsp, unsigned long cr3, unsigned long rbx,
int pv, int argc, char **argv, char *pwd);
THREAD *create_thread_child( void (*main)(), unsigned long rsp, unsigned long cr3, unsigned long rbx,
int pv,int argc, char **argv, char *pwd, THREAD *thread);
THREAD *pthread_create( void (*main)(), unsigned long rsp, int pv, THREAD *thread);
int pthread_join(THREAD *thread);
THREAD *thread(unsigned long pid);


#endif
