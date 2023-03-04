#include <idt.h>
#include <stdio.h>
#include <thread.h>
#include <mm.h>
#include <storage.h>

#include <string.h>
#include <stdlib.h>

#include <io.h>

#include <data.h>

#include <gui.h>

#include <socket.h>


#define SYSCALL_TABLE 35

unsigned long ret;

extern unsigned long server_id[6];
extern PAINT *paint_ready_queue;


extern char *syspwd;
extern void int114(); // system call
extern unsigned long exectve_child(int argc, char **argv, FILE *fp, THREAD *thread );
void default_syscall(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	printf("Syscall %x error\n",rdx); 
	
	
}


void syscall_getpid(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx) {


    if(rcx == 1234) ret = getpid2();
    else ret = getpid();
}


extern void window_add(unsigned long addr, int flag);
void syscall_window(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	window_add(rdi, rsi);
}


void syscall_kbd_foco(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	_stdin = (FILE *)current_thread->stdin;
    _stdout = (FILE *)current_thread->stdout;
    _stderr = (FILE *)current_thread->stderr;
	mouse2 = (mouse_t*) current_thread->mouse;

}


void syscall_alloc_pages(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	unsigned long addr = 0;
	alloc_pages(1, rcx, &addr);
	ret  = addr;
}

void syscall_free_pages(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx) 
{

	free_pages((void*)rdi);
}

unsigned long __malloc_syscall(THREAD *thread, unsigned size) {

	unsigned long addr;
	
	int len = size/0x1000;
	if(size%0x1000) len++;
	
	alloc_pages(1, len, &addr);

    POOL_SYSCALL *m = (POOL_SYSCALL*) thread->pool;

    for(int i=0; i < POOL_COUNT; i++){
        if(m->ptr == 0){
            m->ptr = addr;
            m->size = size;
            m->flag = 0;
            break;
        }

        m++;
    }
	
	return addr;

}


void __free_syscall(THREAD *thread, void *ptr) {

    if(!ptr)return;

	free_pages(ptr);
    POOL_SYSCALL *m = (POOL_SYSCALL*) thread->pool;

    for(int i=0; i < POOL_COUNT; i++){
        if(m->ptr == (unsigned long)ptr){
            m->ptr = 0;
            m->size = 0;
            m->flag = 0;
            break;
        }

        m++;
    }
}

void malloc_syscall(unsigned long rdi) {

    THREAD *thread = current_thread;
    ret = __malloc_syscall(thread, rdi);

}


void free_syscall(unsigned long rdi) {
    THREAD *thread = current_thread;
    __free_syscall(thread, (void *)rdi);
}

void realloc_syscall(unsigned long rdi, unsigned long rsi) {
    THREAD *thread = current_thread;
}



void syscall_readsector(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	ret = read_sector( 0,rcx,rsi,(void*)rdi);
}

void syscall_writesector(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
	ret = write_sector( 0,rcx,rsi,(void*)rdi);

}


void syscall_geral(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx, unsigned long r8 )
{
	
	server_id[0] = rcx; 
	server_id[1] = rdi;
	server_id[2] = rsi; 
	server_id[3] = rdx;
	server_id[4] = current_thread->pid;
	
	if(r8 != 0 && (rcx == 2 || rcx == 1) ) {
		
		strcpy(syspwd,(char*)r8);
		server_id[5] = (unsigned long)syspwd;
	} else server_id[5] = 0;

}

extern int tmpnam;
void syscall_tmpnam(unsigned long rdi) {

	char *s = (char*)rdi;
	
	*s++ = 't';
	*s++ = 'm';
	*s++ = 'p';
	*s = 0;
	
	char str[32];
	
	strcpy(s, itoa(tmpnam++, str));

}

void syscall_io(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx)
{
    ret = -1;

    switch(rcx){
        // in
        case 0:
            ret = inportb( rdi ) &0xff;
        break;
        case 1:
            ret = inportw( rdi ) &0xffff;
        break;
        case 2:
            ret = inportl( rdi ) &0xffffffff;
        break;
        // out
        case 3:
            outportb( rdi, rsi);
            ret = 0;
        break;
        case 4:
            outportw( rdi, rsi);
            ret = 0;
        break;
        case 5:
            outportl( rdi, rsi);
            ret = 0;
        break;

    }
}

void syscall_register_pipe_launcher()
{
    pipe_launcher = current_thread->pipe;
}

void syscall_get_pipe_launcher()
{
    ret = (unsigned long)pipe_launcher;
}


void syscall_get_paint_list()
{
    ret = (unsigned long)paint_ready_queue;
}

void syscall_get_thread_id()
{
    ret = current_thread->id;
}

void syscall_create_socket(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx){
    int domain = (int) rdi;
    int type = (int) rsi;
    int protocol =(int) rcx;

    ret = (unsigned long)socket(domain, type, protocol);
}


void syscall_get_socket_ready_queue()
{
    ret = (unsigned long) saddr_ready_queue;
}

void syscall_pthread_create(unsigned long rdi, unsigned long rsi)
{
    void *start = (void*)rdi;
    ret = (unsigned long) pthread_create( start, rsi, 0x1,current_thread);
}

void syscall_pthread_join(unsigned long rdi)
{
    THREAD *thread = (THREAD *)rdi;
    ret = (unsigned long) pthread_join(thread);
}

void syscall_wait_state(unsigned long rdi)
{
    if(current_thread->tail != 0) {
       current_thread->status |= THREAD_WAIT;
    }
}


void syscall_wait(unsigned long rdi)
{
    ret = 0;
    if(!(current_thread->status&THREAD_WAIT)){
        ret = current_thread->wait_pid;
    }
}


void *fnvetors_syscall[SYSCALL_TABLE] = {
	&default_syscall, 	    // 0
    &syscall_getpid,        // 1
    &syscall_window,	    // 2
    &syscall_kbd_foco,	    // 3
    &syscall_alloc_pages,	// 4
    &syscall_free_pages,	// 5
    &syscall_readsector,	// 6
    &syscall_writesector,	// 7
    &syscall_geral,		    // 8
    &syscall_tmpnam,	    // 9
    &syscall_io,	        // 10
    &syscall_register_pipe_launcher, // 11
    &syscall_get_pipe_launcher, // 12
    &syscall_get_paint_list, // 13
    &syscall_get_thread_id, // 14
    &syscall_create_socket, // 15
    &syscall_get_socket_ready_queue, // 16
    &syscall_pthread_create, // 17
    &syscall_pthread_join, // 18
    &default_syscall, 	    // 19
    &default_syscall, 	    // 20
    &default_syscall, 	    // 21
    &default_syscall, 	    // 22
    &default_syscall, 	    // 23
    &default_syscall, 	    // 24
    &default_syscall, 	    // 25
    &default_syscall, 	    // 26
    &default_syscall, 	    // 27
    &default_syscall, 	    // 28
    &default_syscall, 	    // 29
    &syscall_wait_state,    // 30
    &syscall_wait, 	        // 31
    &malloc_syscall, 	    // 32
    &free_syscall, 	        // 33
    &realloc_syscall        // 34

};



extern void call_function(void *, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);


void syscall_function_handler(unsigned long rdi, unsigned long rsi, unsigned long rdx,
unsigned long rcx, unsigned long r8, unsigned long r9)
{
    	
    	int vetor = rdx;
    	
    	if( rdx >= SYSCALL_TABLE) vetor = 0;
    	
    	void *addr = fnvetors_syscall[vetor];
    	// Chamda de função correspondente
    	call_function(addr, rdi, rsi, rdx, rcx, r8, r9);
    	
}


void syscall_install()
{
	// ring 3  	
    _idt_gate(0x72,(unsigned long)int114, 0x8,3,0);
	
}
