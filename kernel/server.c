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

	_stdin = (FILE *) tmp->stdin;
	mouse2 = (mouse_t*) tmp->mouse;

	window_foco((unsigned long)tmp->window);
	
}

void foco_default()
{
    THREAD *tmp = thread_ready_queue;

    _stdin = (FILE *) tmp->stdin;
	mouse2 = (mouse_t*) tmp->mouse;
}


void server(unsigned long entry_pointer_info)
{
    char *buf;
	char *pwd;
	unsigned long pid;
	
	short pipe[8] = {0,0,0,0,0,0,0,0};
	pipe[0] = 0x1001;
	THREAD *thr;
	
	memset(server_id, 0, sizeof(unsigned long)*5);
	
loop:	
	// wait
	switch(*server_id) {
		case 1:
            cli();
			buf = (char*) server_id[1];
			pid = server_id[4];
			pwd = (char *) server_id[5];
			unsigned long v = server_id[2];
			int argc = 0;
			char **argv = 0;
			if(v) { 
				argc = *(int*)v;
				v += 8;
				argv = (char**) v;
				v += 8*COUNT_ARGV;
				for(int i=0; i < COUNT_ARGV; i ++)
                {
					argv[i] = (char*) v;
					v += 256;
				}
	
			}
					
			if(!exectve_child(argc, argv, pwd, buf, thread(pid)))
            {
				// error
				thr = thread(pid);
				pipe_write ( pipe, thr->pipe);
			}
	
			memset(server_id, 0, sizeof(unsigned long)*5);
			sti();
				break;
		case 2:
            cli();
			buf = (char*) server_id[1];
			thread_id = server_id[2];
			pwd = (char *) server_id[5];
			exectve(0, 0, pwd, buf);
			thread_id = 0;
			memset(server_id, 0, sizeof(unsigned long)*5);
			sti();
				break;
		case 3:
			reboot();
			break;
		case 4:	
			poweroff();
			break;
		case 5:
			cli();
			foco(server_id[4], server_id[2]);
			memset(server_id, 0, sizeof(unsigned long)*5);
			sti();
			break;
		case 6:
			cli();
			pid = server_id[4];
			thr = thread(pid);
			exit(0, thr);
			memset(server_id, 0, sizeof(unsigned long)*5);
			sti();
			break;
		default: 
            // processar socket
            //cli();
            socket_server_transmit();
            socket_execute_row();
            //sti();
			break;
	}

	goto loop;
}














