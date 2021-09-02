#include <window.h>



void fmouse(WINDOW *w)
{

	int x1 = w->pos_x;
	int y1 = w->pos_y;
	int x2 = w->width;
	int y2 = w->height;
	
	int mx = mouse2->x;
	int mb = mouse2->b;
	int my = mouse2->y;
	
	int y, x;
	
	if(mb&0x1) {
	
	if(mx > x1 && my > (y1+WBART_SIZE) &&  mx < (x1+x2) && my < (y1+WBART_SIZE + y2) ) {
	
		// obj
		obj_process(mx - x1 , my - y1 );
		
	
	} else if(mx > x1 && my > y1 &&  mx < (x1+x2) && my < (y1 + y2) ) { // deslocamento de janela
		int dx = (mouse2->x - w->pos_x);
		int dy = (mouse2->y - w->pos_y);
		
		while(mouse2->b&0x1) { // espera soltar
			
			//while(w->spinlock) __asm__ __volatile__("pause;");
			//w->spinlock ++;
			
			
			x = mouse2->x - dx;
			y = mouse2->y - dy;
			
			if(y < 0 ) y = 0;
			if(x < 0 ) x = 0;
			
			w->pos_x = x;
			w->pos_y = y;
			
			//w->spinlock = 0;
			
		}
		
		
	
	}
	
	}
	
	obj_focprocess();

}
