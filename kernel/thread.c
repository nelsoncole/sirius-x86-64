#include <thread.h>
#include <string.h>

#include <paging.h>
#include <mm.h>

#include <stdio.h>
#include <stdlib.h>

#include <gui.h>

#include <data.h>


extern int screan_spin_lock;

extern unsigned long thread_id;
// registers
unsigned long	context_rip, context_rflag, context_cr3;

//registers 
unsigned long	context_rax, context_rbx, context_rcx, context_rdx;

// registers 
unsigned long	context_rdi, context_rsi;

// registers
unsigned long	context_rbp, context_rsp;

// segments
unsigned long	context_cs, context_es, context_ds, context_fs, context_gs, context_ss;

unsigned long context_r15, context_r14, context_r13, context_r12, context_r11, context_r10, context_r9, context_r8;

unsigned long context_pid, context_rsp0;

char *context_fxsave;

/*
 *
 */
// registers
unsigned long	context2_rip, context2_rflag, context2_cr3;

//registers 
unsigned long	context2_rax, context2_rbx, context2_rcx, context2_rdx;

// registers 
unsigned long	context2_rdi, context2_rsi;

// registers
unsigned long	context2_rbp, context2_rsp;

// segments
unsigned long	context2_cs, context2_es, context2_ds, context2_fs, context2_gs, context2_ss;

unsigned long context2_r15, context2_r14, context2_r13, context2_r12, context2_r11, context2_r10, context2_r9, context2_r8;

unsigned long context2_pid, context2_rsp0;

char *context2_fxsave;


THREAD	*current_thread;
THREAD	*thread_ready_queue; // O início da lista.

THREAD	*current_thread1;
THREAD	*current_thread2;


unsigned long next_pid;

extern void SavedFloats(void);
extern void SavedFloats3(void);
extern void SaveSSE(void *f);
int first_thread2;

unsigned long thread_setup() {

	next_pid = 0;
    first_thread2 = 0;
	unsigned long addr = 0;
	context_fxsave = (char*) (unsigned long)&SavedFloats;
    context2_fxsave = (char*) (unsigned long)&SavedFloats3;
    alloc_pages(0, 2, &addr);
	thread_ready_queue = (THREAD*)addr;
    current_thread2 = current_thread 	= thread_ready_queue;
    	
    memset(current_thread, 0, sizeof(THREAD)); 
    	
    current_thread->pid 	= next_pid++;
    	
   	SaveSSE(current_thread->fxsave);
    	
    current_thread->cs 	= 0x8;
    //current_thread->ds 	= 0x10;
    //current_thread->es 	= 0x10;
    //current_thread->fs 	= 0x10;
    //current_thread->gs 	= 0x10;
    current_thread->ss 	= 0x10;

	current_thread->rflag 	=0x202;
	
	current_thread->cr3 = (unsigned long) pml4e;
	
	
	current_thread->stdin = fopen (0, "stdin");
	current_thread->stdout = fopen (0, "stdout");
	current_thread->stderr = fopen (0, "stderr");
	
	current_thread->pipe = fopen (0, "pipe");
	
	current_thread->mouse = (mouse_t*) malloc(sizeof(mouse_t));
	
    current_thread->head    = 0;

	current_thread->prv	    = 0;

   
    current_thread->next 	= 0;
    	
    current_thread1 = current_thread;
    current_thread2	= current_thread; 

	return (current_thread->pid);

}



THREAD *create_thread( void (*main)(), unsigned long rsp, unsigned long cr3, unsigned long rbx,
int pv,int argc, char **argv, char *pwd)
{
	unsigned long v = 0;
    alloc_pages(0, 2, (unsigned long *)&v); 
    THREAD	*new_thread =(THREAD*)v;//malloc(sizeof(THREAD));
    memset(new_thread, 0, sizeof(THREAD));
    new_thread->pid 	= next_pid++;


	SaveSSE(new_thread->fxsave);
    
    new_thread->rsp 	= rsp;
	new_thread->rip  	= (unsigned long)main;

    if((pv&1) == 1) { // processo do userspace
    	new_thread->cs		= 0x1B;
   		//new_thread->ds 	= 0x23;
   		//new_thread->es 	= 0x23;
   		//new_thread->fs 	= 0x23;
   		//new_thread->gs 	= 0x23;
   		new_thread->ss 	= 0x23;
    		
   		unsigned long addr;
		alloc_pages(1, 1 + ((28*256)/0x1000), (unsigned long *)&addr);
		memset((char*)addr, 0, ((28*256)/0x1000) + 0x1000);
		new_thread->alloc_addr = addr;
		
		new_thread->prv = 1;
    
        new_thread->rflag 	= 0x3202;

   	} else if(!(pv&1)) { // processo do kernelspace
   		new_thread->cs 	= 0x8;
   		//new_thread->ds 	= 0x10;
   		//new_thread->es 	= 0x10;
   		//new_thread->fs 	= 0x10;
   		//new_thread->gs 	= 0x10;
   		new_thread->ss 	= 0x10;
		new_thread->prv = 0;

        new_thread->rflag 	= 0x202;

    }
    	
    if((pv&0x80) == 0x80) {
    	
    	new_thread->prv |= 0x80;
    			
    }

    new_thread->cr3 	= cr3;
    	
    	
    new_thread->stdin = fopen (0, "stdin");
	new_thread->stdout = fopen (0, "stdout");
	new_thread->stderr = fopen (0, "stderr");
	new_thread->pipe = fopen (0, "pipe");
	
	new_thread->mouse = (mouse_t*) malloc(sizeof(mouse_t));
	
	new_thread->prv |= 0x20; // e pai
	
	
	if(pv&0x10) {
	
		alloc_pages(1, 512*3 /*6MiB*/, (unsigned long *)&v);
		new_thread->window = (unsigned long ) v; 
		
		new_thread->prv |= 0x10;
		
		WINDOW *w = (WINDOW *)v;
		memset(w,0,0x1000);
		
		w->rx = gui->horizontal_resolution;
		w->ry = gui->vertical_resolution;
		w->bpp = gui->bpp;
		w->scanline = gui->pixels_per_scan_line;
		
		new_thread->id = thread_id;
		
	}
	
	
	
	new_thread->rcx	= (unsigned long)new_thread->stdin;
	new_thread->rdx	= (unsigned long)new_thread->stdout;
	new_thread->rsi	= (unsigned long)new_thread->stderr;
	new_thread->rdi	= (unsigned long)new_thread->window;
	new_thread->rbx	= rbx;
	new_thread->r8 = 0; // reserved
	new_thread->r9		= new_thread->alloc_addr;
	new_thread->r10	= (unsigned long)new_thread->pipe;
	
	new_thread->rax	= (unsigned long)new_thread->mouse;
	
	new_thread->r11 = argc;
	new_thread->r12 = (unsigned long)argv;
	new_thread->r13 = (unsigned long)pwd;

    new_thread->head    = NULL;
    new_thread->next 	= NULL;
    new_thread->tail 	= NULL;
    

    // Adicionar novo elemento, no final da lista
    // tmp aponta para inicio da lista
    THREAD *tmp = thread_ready_queue;
    while (tmp->next) {
    	
    	tmp = tmp->next;
    }
    	
    tmp->next = new_thread;
    
   	return (new_thread);
}

THREAD *create_thread_child( void (*main)(), unsigned long rsp, unsigned long cr3, unsigned long rbx, int pv,
int argc, char **argv, char *pwd, THREAD *thread)
{
	THREAD	*t = thread;
    unsigned long v = 0;
    alloc_pages(0, 2, (unsigned long *)&v); 
    THREAD	*new_thread =(THREAD*)v;//malloc(sizeof(THREAD));
    memset(new_thread, 0, sizeof(THREAD));
    new_thread->pid 	= next_pid++;


	SaveSSE(new_thread->fxsave);
    
    new_thread->rsp 	= rsp;
	new_thread->rip  	= (unsigned long)main;

    if((pv&1) == 1) { // processo do userspace
    	new_thread->cs		= 0x1B;
    	//new_thread->ds 	= 0x23;
    	//new_thread->es 	= 0x23;
    	//new_thread->fs 	= 0x23;
    	//new_thread->gs 	= 0x23;
    	new_thread->ss 	    = 0x23;

		unsigned long addr;
		alloc_pages(1, 1 + ((28*256)/0x1000), (unsigned long *)&addr);
		memset((char*)addr, 0, ((28*256)/0x1000) + 0x1000);
		new_thread->alloc_addr = addr;
		 
		new_thread->prv = 1;

        new_thread->rflag 	= 0x3202;

    } else if(!(pv&1)) { // processo do kernelspace
    	new_thread->cs 	    = 0x8;
    	//new_thread->ds 	= 0x10;
    	//new_thread->es 	= 0x10;
    	//new_thread->fs 	= 0x10;
    	//new_thread->gs 	= 0x10;
   		new_thread->ss 	    = 0x10;
		new_thread->prv     = 0;

        new_thread->rflag 	= 0x202;
   	}
   
    new_thread->cr3    = cr3;
    	
    	
    new_thread->stdin  = t->stdin;
	new_thread->stdout = t->stdout;
	new_thread->stderr = t->stderr;
	new_thread->pipe   = t->pipe;
	
	new_thread->mouse  = t->mouse;
	
	new_thread->window = t->window; 

	
	new_thread->rcx	= (unsigned long)new_thread->stdin;
	new_thread->rdx	= (unsigned long)new_thread->stdout;
	new_thread->rsi	= (unsigned long)new_thread->stderr;
	new_thread->rdi	= (unsigned long)new_thread->window;
	new_thread->rbx	= rbx;
	new_thread->r8 = 0; // reserved
	new_thread->r9		= new_thread->alloc_addr;
	new_thread->r10	= (unsigned long)new_thread->pipe;

	new_thread->rax	= (unsigned long)new_thread->mouse;
	
	new_thread->r11 = argc;
	new_thread->r12 = (unsigned long)argv;
	new_thread->r13 = (unsigned long)pwd;

    new_thread->head    = thread;
    new_thread->next 	= NULL;
    new_thread->tail 	= NULL;
    

    // Adicionar novo elemento, no final da lista
    // tmp aponta para inicio da lista
    THREAD *tmp = thread_ready_queue;
    while (tmp->next) {
    	
    	tmp = tmp->next;
    }
    	
    tmp->next = new_thread;
    t->tail = new_thread;

   	return (new_thread);
}

THREAD *pthread_create( void (*main)(), unsigned long rsp, int pv, THREAD *thread)
{
	THREAD	*t = thread;
    unsigned long v = 0;
    alloc_pages(0, 2, (unsigned long *)&v); 
    THREAD	*new_thread =(THREAD*)v;//malloc(sizeof(THREAD));
    memset(new_thread, 0, sizeof(THREAD));
    new_thread->pid 	= next_pid++;

	SaveSSE(new_thread->fxsave);
    
    new_thread->rsp 	= rsp;
	new_thread->rip  	= (unsigned long)main;
    new_thread->status  = THREAD_ZUMBI;
    if((pv&1) == 1) { // processo do userspace
    	new_thread->cs		= 0x1B;
    	//new_thread->ds 	= 0x23;
    	//new_thread->es 	= 0x23;
    	//new_thread->fs 	= 0x23;
    	//new_thread->gs 	= 0x23;
    	new_thread->ss 	    = 0x23;

		new_thread->alloc_addr = thread->alloc_addr;
		 
		new_thread->prv = 1;

        new_thread->rflag 	= 0x3202;

    } else if(!(pv&1)) { // processo do kernelspace
    	new_thread->cs 	    = 0x8;
    	//new_thread->ds 	= 0x10;
    	//new_thread->es 	= 0x10;
    	//new_thread->fs 	= 0x10;
    	//new_thread->gs 	= 0x10;
   		new_thread->ss 	    = 0x10;
		new_thread->prv     = 0;

        new_thread->rflag 	= 0x202;
   	}
   
    new_thread->cr3    = thread->cr3;
    	 	
    new_thread->stdin  = t->stdin;
	new_thread->stdout = t->stdout;
	new_thread->stderr = t->stderr;
	new_thread->pipe   = t->pipe;
	
	new_thread->mouse  = t->mouse;
	
	new_thread->window = t->window; 

	
	new_thread->rcx	= (unsigned long)new_thread->stdin;
	new_thread->rdx	= (unsigned long)new_thread->stdout;
	new_thread->rsi	= (unsigned long)new_thread->stderr;
	new_thread->rdi	= (unsigned long)new_thread->window;
	new_thread->rbx	= 0;//rbx;
	new_thread->r8 = 0; // reserved
	new_thread->r9		= new_thread->alloc_addr;
	new_thread->r10	= (unsigned long)new_thread->pipe;

	new_thread->rax	= (unsigned long)new_thread->mouse;
	
	new_thread->r11 = 0;//argc;
	new_thread->r12 = (unsigned long)0; //argv;
	new_thread->r13 = (unsigned long)0; // pwd

    new_thread->next 	= NULL;
    new_thread->tail 	= NULL;
    

    // Adicionar novo elemento, no final da lista
    // tmp aponta para inicio da lista
    THREAD *tmp = thread_ready_queue;
    while (tmp->next) {
    	
    	tmp = tmp->next;
    }
    	
    tmp->next = new_thread;
    t->tail = new_thread;

   	return (new_thread);
}

int pthread_join(THREAD *thread){
    thread->status &=~THREAD_ZUMBI;
    return 0;
}


void task_switch()
{
    	
    //current_thread = current_thread1;

    // Salve o contexto da actual tarefa
    // em execução   
	current_thread->rax 	= context_rax;
    current_thread->rbx 	= context_rbx;
    current_thread->rcx 	= context_rcx;
    current_thread->rdx 	= context_rdx;
    current_thread->rdi 	= context_rdi;
    current_thread->rsi 	= context_rsi;
    current_thread->rbp 	= context_rbp;
    current_thread->rsp 	= context_rsp;
    current_thread->rip 	= context_rip;
    	
    current_thread->r8 	    = context_r8;
    current_thread->r9 	    = context_r9;
    current_thread->r10 	= context_r10;
    current_thread->r11 	= context_r11;
    current_thread->r12 	= context_r12;
    current_thread->r13 	= context_r13;
    current_thread->r14 	= context_r14;
    current_thread->r15 	= context_r15;

	current_thread->cs 	    = context_cs;
    current_thread->ds 	    = context_ds;
    current_thread->es 	    = context_es;
    current_thread->fs 	    = context_fs;
    current_thread->gs 	    = context_gs;
    current_thread->ss 	    = context_ss;

	current_thread->rflag 	= context_rflag;
    current_thread->cr3 	= context_cr3;
    	
    // sse
    //memcpy(current_thread->fxsave, context_fxsave, 512);
    	
    for(int i=0;i<512;i++) current_thread->fxsave[i] = context_fxsave[i];
    	
    
	// Obter a próxima tarefa a ser executada.
	current_thread 	= current_thread->next;
	 while(current_thread){
        if((current_thread->prv & 0x80) || current_thread->status == THREAD_ZUMBI) {
		    current_thread = current_thread->next;
        }else break;
    }

    // Se caímos no final da lista vinculada, 
    // comece novamente do início.
    if (!current_thread) {

    	current_thread = thread_ready_queue;
    }

	//current_thread1 	= current_thread;


    // Restaura o contexto da próxima
    // tarefa a ser executada...
	context_rax 	= current_thread->rax;
    context_rbx 	= current_thread->rbx;
    context_rcx 	= current_thread->rcx;
    context_rdx 	= current_thread->rdx;
    context_rdi 	= current_thread->rdi;
   	context_rsi 	= current_thread->rsi;
    context_rbp 	= current_thread->rbp;
    context_rsp 	= current_thread->rsp;
    context_rip 	= current_thread->rip;
    	
    context_r8 	= current_thread->r8;
    context_r9 	= current_thread->r9;
    context_r10 	= current_thread->r10;
    context_r11 	= current_thread->r11;
    context_r12 	= current_thread->r12;
    context_r13 	= current_thread->r13;
    context_r14 	= current_thread->r14;
    context_r15 	= current_thread->r15;

	context_cs 	= current_thread->cs;
    context_ds	= current_thread->ds;
    context_es	= current_thread->es;
    context_fs	= current_thread->fs;
    context_gs	= current_thread->gs;
    context_ss	= current_thread->ss;

	context_rflag	= current_thread->rflag;
    context_cr3	= current_thread->cr3;
    	
    	
    context_pid	= current_thread->pid;
    	
    /*
    if(current_thread->pid > 5) {
    	while(screan_spin_lock);
	screan_spin_lock++;
		
	for(int i=0; i < 512; i++)
		printf("%d ", context_fxsave[i]);
    	
   		for(;;);
   	}*/


	for(int i=0;i<512;i++) context_fxsave[i] = current_thread->fxsave[i];
	// memcpy(context_fxsave, current_thread->fxsave, 512);

    // stack rsp0
	// if(context_rsp0)tss_rsp0(context_rsp0);

}

void task_switch2()
{
	THREAD *tmp = current_thread2;	
    if(!first_thread2){
        goto __no;
    }

    // Salve o contexto da actual tarefa
    // em execução   
	tmp->rax 	= context2_rax;
    tmp->rbx 	= context2_rbx;
    tmp->rcx 	= context2_rcx;
    tmp->rdx 	= context2_rdx;
    tmp->rdi 	= context2_rdi;
    tmp->rsi 	= context2_rsi;
    tmp->rbp 	= context2_rbp;
    tmp->rsp 	= context2_rsp;
    tmp->rip 	= context2_rip;
    	
    tmp->r8 	= context2_r8;
    tmp->r9 	= context2_r9;
    tmp->r10 	= context2_r10;
    tmp->r11 	= context2_r11;
    tmp->r12 	= context2_r12;
    tmp->r13 	= context2_r13;
    tmp->r14 	= context2_r14;
    tmp->r15 	= context2_r15;

	tmp->cs 	= context2_cs;
    tmp->ds 	= context2_ds;
    tmp->es 	= context2_es;
    tmp->fs 	= context2_fs;
    tmp->gs 	= context2_gs;
    tmp->ss 	= context2_ss;

	tmp->rflag 	= context2_rflag;
    tmp->cr3 	= context2_cr3;

    for(int i=0;i<512;i++) tmp->fxsave[i] = context2_fxsave[i];

__no:  
    // next
    tmp 	= tmp->next;
    int i;
    int x = 0;
    for(i=0; i < 2; i++){
        while(tmp) {
		    if((tmp->prv&0x80) && (tmp->status != THREAD_ZUMBI)) {
                x = 1;
			    break;
            }
		    tmp 	= tmp->next;
	    }

        if(x == 1) break;

        if(tmp) break;

        tmp = thread_ready_queue;
	}

	if(tmp == 0 || i >= 2) return;

	current_thread2 	= tmp;
    first_thread2       = 1;

    // Restaura o contexto da próxima
    // tarefa a ser executada...
	context2_rax 	= tmp->rax;
    context2_rbx 	= tmp->rbx;
    context2_rcx 	= tmp->rcx;
    context2_rdx 	= tmp->rdx;
    context2_rdi 	= tmp->rdi;
    context2_rsi 	= tmp->rsi;
    context2_rbp 	= tmp->rbp;
    context2_rsp 	= tmp->rsp;
    context2_rip 	= tmp->rip;
    	
    context2_r8     = tmp->r8;
    context2_r9     = tmp->r9;
    context2_r10 	= tmp->r10;
    context2_r11 	= tmp->r11;
    context2_r12 	= tmp->r12;
    context2_r13 	= tmp->r13;
    context2_r14 	= tmp->r14;
    context2_r15 	= tmp->r15;

	context2_cs 	= tmp->cs;
    context2_ds	    = tmp->ds;
    context2_es	    = tmp->es;
    context2_fs	    = tmp->fs;
    context2_gs	    = tmp->gs;
    context2_ss	    = tmp->ss;

	context2_rflag	= tmp->rflag;
    context2_cr3	= tmp->cr3;
    	
    context2_pid	= tmp->pid;

    // stack rsp0
	// if(context_rsp0)tss_rsp0(context_rsp0);

    for(int i=0;i<512;i++) context2_fxsave[i] = tmp->fxsave[i];

}

unsigned long getpid(){
    return current_thread->pid;
}

unsigned long getpid2(){

    THREAD	*thread = current_thread->head;
    if(!thread){
        thread = current_thread;
    }

    return thread->pid;
}

THREAD *get_thread(unsigned long pid)
{
	THREAD *t = thread_ready_queue;
	
	while(t) {
	
		if(t->pid == pid) return (t);
		
		t = t->next;
	}

	return (0);
}

