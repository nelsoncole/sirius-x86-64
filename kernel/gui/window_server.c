#include <stdio.h>
#include <gui.h>
#include <data.h>

extern PAINT *paint_foco;
extern PAINT *paint_ready_queue;
extern int launcher;
extern int screan_spin_lock;

int w_remove_spinlock;

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
            int dx = (mouse->x - w->pos_x);
		    int dy = (mouse->y - w->pos_y);
            while(mouse->b&0x1) { // espera soltar
	
			    int x = mouse->x - dx;
			    int y = mouse->y - dy;
			
			    if(y < 0 ) y = 0;
			    if(x < 0 ) x = 0;
			
			    w->pos_x = x;
			    w->pos_y = y;
		    }

        }

        w_remove_spinlock = 0;
    }

    while(1){}
}
