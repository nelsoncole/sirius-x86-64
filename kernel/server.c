#include <stdio.h>
#include <thread.h>
#include <kernel.h>
#include <io.h>
#include <string.h>
#include <acpi.h>

#include <data.h>

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

	
	FILE *file;
	char *path;
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
			path = (char*) server_id[1];
			pid = server_id[4];
			pwd = (char *) server_id[5];
			unsigned long v = server_id[2];
			cli();
			file = fopen(path,"r+b");
			if(!file){
				printf("open error \"%s\"\n",path); 
				
				thr = thread(pid);
				pipe_write ( pipe, thr->pipe);
				/* for(;;);*/ 
			}else {
			
			
				int argc = 0;
				char **argv = 0;
				if(v) { 
					argc = *(int*)v;
					v += 8;
					argv = (char**) v;
					v += 8*COUNT_ARGV;
					for(int i=0; i < COUNT_ARGV; i ++){
						argv[i] = (char*) v;
						v += 256;
					}
	
				}
					
				if(!exectve_child(argc, argv, pwd, file, thread(pid))){
					// error
					thr = thread(pid);
					pipe_write ( pipe, thr->pipe);
				}
				fclose(file);
				
			}
			memset(server_id, 0, sizeof(unsigned long)*5);
			sti();
				break;
		case 2:
			path = (char*) server_id[1];
			thread_id = server_id[2];
			pwd = (char *) server_id[5];
			
			cli();
			file = fopen(path,"r+b");
			if(!file){printf("open error \"%s\"\n",path);  /*for(;;);*/ }
			else {
		
				exectve(0, 0, pwd, file);
				thread_id = 0;
				fclose(file);
				
			}
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
			break;
	}

	goto loop;
}














