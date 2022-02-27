#include <stdio.h>
#include <gui.h>
#include <data.h>

extern PAINT *paint_foco;
extern PAINT *paint_ready_queue;
extern int launcher;
extern int screan_spin_lock;

int w_remove_spinlock;
int window_putchar( unsigned short int c, WINDOW *window);
void window_server(){
    int tx = 0;
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

            if(mb&0x1) {

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
			
			            w->pos_x = x;
			            w->pos_y = y;
   		            }   
                }
            }

            // terminal
            if(w->terminal == 0x1234){
                FILE *fp = (FILE *)w->stdin;
                if(fp != 0)
                if(fp->off > tx){
                    fp->curp = (unsigned char*)(fp->buffer + tx);
		            unsigned short int c = *(unsigned char*)(fp->curp) &0xff;
                    window_putchar( c, w);
                    tx++;
                }
            }
        }

        w_remove_spinlock = 0;
    }

    while(1){}
}


void put_pixel_sirius(long x, long y, unsigned int scanline,unsigned int color, WINDOW *window)
{
	WINDOW *w = (WINDOW*)window;
    if(!window) return;
	
	if(x >= scanline || y >= w->ry) return;
	
	unsigned int *buf = (unsigned int*)&w->start;

	*(unsigned int*)((unsigned int*)buf + (scanline * y) + x) = color;
	
}


void drawchar( unsigned short int c, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w)
{
	unsigned short f = 0;
	const unsigned short *font_data = (unsigned short*) font->buf;
	unsigned short mask;
	//unsigned int color = font->fg_color;
	
	if(c > 127) return; // TODO
    
   	for(int y=0; y < font->y ; y++){
		mask = 1;
        	f = font_data[(c *font->y) + y];
		
                for(int x = font->x -1 ; x >= 0; x--){ 
                       if(f&mask){
				put_pixel_sirius(cx + x,cy + y, w->width, fg, w);
				
			}else{
				put_pixel_sirius(cx + x,cy + y, w->width, bg, w);
			}
			mask += mask;
                  
          }
        
    }

}

int window_putchar( unsigned short int c, WINDOW *window)
{

	WINDOW *w = (WINDOW*)window;
    if(!w)return 0;
	
	int limitx = (w->area_width-4)/w->font.x;
	int limity = (w->area_height-4)/w->font.y;
	
	w->font.fg_color = w->text_fg;

	if(w->cx >= limitx) {
		w->cx =0;
		w->cy++;
	}
		
	if(w->cy >= limity){
	
		//scroll();
		w->cy = w->cx =0;
		//__window_clear(w);
		return c;
		
	}

	if(c == '\b' && (w->cx > 0))
	{
		w->cx--;
		drawchar(' ', 4 + w->area_x + w->font.x*w->cx, 4 + w->area_y + w->font.y*w->cy, 
		w->font.fg_color, w->font.bg_color, &w->font, w);
		//w->cx++;
	} else if(c == '\t') {
		w->cx += 8;
		
	} else if(c == '\n') {
	
		w->cx = 0;
		w->cy++;
		
	} else if(c >= ' ') {
	
		drawchar( c, 4 + w->area_x + w->font.x*w->cx, 4 + w->area_y + w->font.y*w->cy, 
		w->font.fg_color, w->font.bg_color, &w->font, w);
		w->cx++;
	}
	
	return c;
}
