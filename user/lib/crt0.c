#include <stdio.h>
#include <stdlib.h>

#include <alloc.h>
#include <window.h>
#include <data.h>

#define COUNT_ARGV 32

extern int main(int argc, char **argv);

unsigned long __window;

FILE *stdin;
FILE *stdout;
FILE *stderr;


FILE *__pipe__;
unsigned long pipe_r;
unsigned long ptr_mouse2;

long	obj_list;
extern unsigned long argc;
extern unsigned long argv;
extern unsigned long pwd_ptr;
mouse_t *mouse;
mouse_t *mouse2;
user_t *user;
unsigned long *__tm;

char *pwd;

extern int term_fg_color;
extern int term_bg_color;

void ctr0(unsigned long rdi, unsigned long rsi, unsigned long rdx, unsigned long rcx,
unsigned long r8, unsigned long r9) {
	
	__window = rdi;
	user 	= (user_t*) r8;
	mouse 	= (mouse_t*) user->mouse;
	mouse2	= (mouse_t*) ptr_mouse2;
	__tm	= (unsigned long*) user->clock;
	
	stdin 	= (FILE *) rcx;
	stdout	= (FILE *) rdx;
	stderr	= (FILE *) rsi;
	
	__pipe__= (FILE *) (unsigned long) pipe_r;
	
	// objectos
	obj_list = 0;
	
	pwd = (char *) pwd_ptr;


    term_fg_color = term_bg_color = -1;

	int ret = main(argc, (char**)argv);
	
	exit(ret);

}
