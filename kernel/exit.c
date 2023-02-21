#include <thread.h>
#include <paging.h>
#include <mm.h>

#include <stdio.h>
#include <stdlib.h>

#include <data.h>


extern unsigned char mult_task;
extern int pipe_write ( void *buf, FILE *fp);
extern void window_remove(unsigned long addr);
extern void foco_default();
extern int pipe_write ( void *buf, FILE *fp);

void user_free(THREAD *thr )
{
	POOL_SYSCALL *m = (POOL_SYSCALL *)thr->pool;

	for(int i=0; i <POOL_COUNT; i++){
		int *ptr = (int*)m->ptr;
		if(ptr != 0)free_pages(ptr);
		m++;
	}

	m = (POOL_SYSCALL *)thr->pool;
	free_pages(m);
}


void exit(int rc, THREAD *thr) {

	// A thread deve ser do user-space

	THREAD *tmp = thr;
	// macar todas como zumbi
	while(tmp) {
	 	tmp->status |= THREAD_ZUMBI;
	 	tmp = tmp->tail;
	}
	
	// enviar memsagem dizendo que terminou.
	short pipe[8] = {0,0,0,0,0,0,0,0};
	pipe[0] = 0x1001;
	pipe_write ( pipe, thr->pipe);

	// dealocar todas as memorias
	tmp = thr;
	while(tmp) {
        pae_page_table_t *_pte = (pae_page_table_t *)tmp->pte_addr;
	 
	    for(int i=0; i < tmp->frame_size; i ++) {
	        unsigned long phy = _pte->frames << 12 &0xffffffffffff;
	        free_frame(phy);
	        _pte ++;	
	    }
	
	    free_pages((void*)tmp->pml4e_addr);
	    free_pages((void*)tmp->pdpte_addr);
	    free_pages((void*)tmp->pde_addr);
	    free_pages((void*)tmp->pte_addr);
	 
	    if(tmp->prv&0x10) {
	        // remover window 
            window_remove(tmp->window);
	        free_pages((void*)tmp->window);
	    }
	 
        if(tmp->prv&0x20){
	        // se tem o foco, mudar para padrao
            foco_default();

            fclose(tmp->stdin);
            fclose(tmp->stdout);
	        fclose(tmp->stderr);
	        fclose(tmp->pipe);
	        free(tmp->mouse);

            // Avizar ao launcher que o janela terminou
            // pipe_write ( void *buf, FILE *fp);
           	pipe[0] = 0x1001;
	        pipe[1] = 0;
            pipe[2] = 0;
            pipe[3] = 0;
            pipe[4] = tmp->id &0xffff;
            pipe[5] = tmp->id >> 16 &0xffff;
            pipe[6] = tmp->id >> 32 &0xffff;
            pipe[7] = tmp->id >> 48 &0xffff;

	        pipe_write ( pipe, pipe_launcher);
	    }

        user_free(tmp);	 	
	    tmp = tmp->tail;
	}
	
	
	// remover da lista
	mult_task = 0; // nao
	tmp = thr;

    // processo filho, remover tail
    if( !(tmp->prv&0x20)) {
        THREAD *tail = thread_ready_queue; 
	    while (tail) {
		    if(tail->tail == tmp) {
		        tail->tail = 0;
		        break;
		    }
    		tail = tail->next;
    	}
        
    }
    // processo pai, remover
	while(tmp) {
	    unsigned long addr = (unsigned long) tmp;
	 
	    THREAD *list = thread_ready_queue; 
	    while (list) {
		    if(list->next == tmp) {
		        list->next = tmp->next;
				// reset Thread wait
				THREAD *father = tmp->head;
				father->status &=~THREAD_WAIT;
				// clear tail
				father->tail = 0;
		        break;
		    }
    		list = list->next;
    	}
	 
	    tmp = tmp->tail;	 
	    free_pages((void*)addr);
    }
	
	mult_task = 1; // sim
}
