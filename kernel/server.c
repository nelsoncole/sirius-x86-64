#include <stdio.h>
#include <thread.h>
#include <kernel.h>
#include <io.h>
#include <string.h>
#include <acpi.h>

#include <data.h>

#include <socket.h>
#include <ethernet.h>

unsigned long thread_id;
unsigned long server_id[6];

extern int pipe_write ( void *buf, FILE *fp);
extern void exit(int rc, THREAD *thr);
extern void window_foco(unsigned long addr);
void foco(unsigned long pid, unsigned long id){

	THREAD *tmp = thread_ready_queue;
	 
	while (tmp) {

		if(tmp->id == id) 
			break;
        tmp = tmp->next;
    }
    	
    if(!tmp) return;

	_stdin  = (FILE *) tmp->stdin;
    _stdout = (FILE *) tmp->stdout;
    _stderr = (FILE *) tmp->stderr;
	mouse2 = (mouse_t*) tmp->mouse;

	window_foco((unsigned long)tmp->window);
	
}

void foco_default()
{
    THREAD *tmp = thread_ready_queue;

    _stdin  = (FILE *) tmp->stdin;
    _stdout = (FILE *) tmp->stdout;
    _stderr = (FILE *) tmp->stderr;
	mouse2  = (mouse_t*) tmp->mouse;
}


void server(unsigned long entry_pointer_info)
{
   
    while(1){
        // processar socket
        //cli();
        socket_server_transmit();
        socket_execute_row();
        //sti();
    }	
}














