#include <stdio.h>
#include <gui.h>
#include <data.h>
#include <io.h>

extern int threadz;
typedef struct _W_HANDLE {
	unsigned int 	type;
	unsigned int 	x, y;
	unsigned int 	width, height;
	 
	unsigned int	id, flag;
	unsigned int 	fg, bg;
	unsigned int 	cx, cy; 
	unsigned int	offset;
	unsigned int 	len;
	unsigned int 	bytes;
    unsigned int 	scroll;
	unsigned long	addr;
	unsigned long	w;
}__attribute__ ((packed)) W_HANDLE_T;

extern PAINT *paint_foco;
extern PAINT *paint_ready_queue;
extern int launcher;
extern int screan_spin_lock;

int w_remove_spinlock;
extern int window_putchar( unsigned short int c, unsigned char color, WINDOW *window, int foco);
extern void terminal_mouse_click(int x, int y, XCOMPONENT *com, WINDOW *w);
void window_server_handle(WINDOW *w, int foco);

void window_server(){

    w_remove_spinlock = 0;
    while(launcher != 0){}

    while(1){
        while(w_remove_spinlock != 0){}
        w_remove_spinlock++;

        if(paint_foco){
            WINDOW *w = (WINDOW*) paint_foco->w;
            if(!w){
                w_remove_spinlock = 0;
                continue;
            }

            int x1 = w->pos_x;
	        int y1 = w->pos_y;
	        int x2 = w->width;
	        int y2 = w->height;

            int mx = mouse->x;
	        int mb = mouse->b;
	        int my = mouse->y;

            if(mb&0x1) { // entrar se o botao esquerdo do mouse foi pressionado
                // deslocar a janela
                // deslocar o scroll
                if(mx > x1 && my > y1 &&  mx < (x1+x2) && my < (y1 + 24/*y2*/) ) 
                {
                    // deslocamento de janela
                    int dx = (mouse->x - w->pos_x);
		            int dy = (mouse->y - w->pos_y);
                    while(mouse->b&0x1) { // espera soltar
	
			            int x = mouse->x - dx;
			            int y = mouse->y - dy;
			
			            if(y < 0 ) y = 0;
			            if(x < 0 ) x = 0;
                        threadz = 1;
			            w->pos_x = x;
			            w->pos_y = y;
                        threadz = 0;
   		            } // 3   
                } // 2 
            } // 1 

            if(mb&0x2) { // entrar se o botao direito do mouse foi pressionado
                // destruir a janela
                // tratamento do foco dos objectos na janela
                // enviar mensagem no aplicativo
                
            } // 1

            // operacao do objecto com o foco
            int foco = 0; //w->foco;
            while(*(unsigned char *)&w->spinlock != 0){};
            while( foco < 256)
            {
                window_server_handle(w, foco++);
            }
            *(unsigned char *)&w->spinlock = 0;
        }

        w_remove_spinlock = 0;
    }

    while(1){}
}

// 1 = terminal
void window_server_handle(WINDOW *w, int foco)
{
    int c;
    int mx = mouse->x - w->pos_x;
	int mb = mouse->b;
	int my = mouse->y - w->pos_y;

    XCOMPONENT *com = (XCOMPONENT *)w->component[foco];
    if(!com) return;
    if(com->magic != 1234)return;

    switch(com->type)
    {
        case 1:
            c = fgetc((FILE *)w->stdout);
            if(c != EOF && c != 0)
            {   threadz = 1;
                window_putchar( c &0xff, (c >> 8) &0xff, w, 0);
                threadz = 0;
            }
            // evento onclick
            if(foco == w->foco && (mb&0x1) != 0)
            {
                while(mouse->b&0x1) 
                { // espera soltar
                    mx = mouse->x - w->pos_x;
	                my = mouse->y - w->pos_y;
                    threadz = 1;
                    terminal_mouse_click(mx, my, com, w);
                    threadz = 0;
                }
            }
        break;
    }
}