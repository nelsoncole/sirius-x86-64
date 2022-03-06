#include <stdio.h>
#include <gui.h>
#include <data.h>
#include <io.h>

extern PAINT *paint_foco;
extern PAINT *paint_ready_queue;
extern int launcher;
extern int screan_spin_lock;

int w_remove_spinlock;
int window_putchar( unsigned short int c, unsigned char color, WINDOW *window, FILE *fd);
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
                int c = fgetc((FILE *)w->stdout);
                if(c != EOF && c != 0){
                    while(paint_ready_queue->spinlock){}
	                paint_ready_queue->spinlock++;
                    window_putchar( c &0xff, (c >> 8) &0xff, w, (FILE *)w->stdout);
                    paint_ready_queue->spinlock = 0;
                }
            }
        }

        w_remove_spinlock = 0;
    }

    while(1){}
}



static unsigned int color_table[] = {0x000000,0x0000FF,0x00FF00,0x00FFFF,0xFF0000,0xAA00AA,\
0xAA5500,0x808080,0x555555,0x5555FF,0x55FF55,0x55FFFF,0xFF5555,0xFF55FF,0xFFFF00,0xFFFFFF};

void put_pixel_sirius(long x, long y, unsigned int scanline,unsigned int color, WINDOW *window)
{
	WINDOW *w = (WINDOW*)window;
    if(!window) return;
	
	if(x >= scanline || y >= w->ry) return;
	
	unsigned int *buf = (unsigned int*)((unsigned long)&w->start);

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

void drawline(int x1, int y1, int x2, int y2, int rgb, WINDOW *w)
{

	for(int y=0; y < y2; y++ ) {
		for(int x=0; x < x2; x++) {
			put_pixel_sirius(x1+x, y1+y, w->width, rgb, w);
		}
	}	
}

static int window_putchar_scroll(WINDOW *window, FILE *fd){
    WINDOW *w = (WINDOW*)window;
    if(!w)return 0;

    int bsize = fd->bsize;
    int fsize = fd->fsize;
    unsigned short *buffer = (unsigned short*)fd->buffer;

    // clear
    drawline(w->area_x , w->area_y, w->area_width, w->area_height, w->bg, w);

    // eliminar linha...
    unsigned short *c = buffer;
    int i = 0;
    int scrolly = w->scrolly;

    for(i=w->chcounter; i > 0; --i){
        unsigned short val = *c++;
        unsigned char a = val &0xff;
        unsigned char b = (val >> 8) &0xff;

        if( a == '\n' || b == '\n' ){
            if(!scrolly)
                break;

            scrolly--;
        }
    }

    w->cx =0;
    w->cy =0;

    if(i <= 0){ return 0;}

    for( ;i > 1; --i){

        unsigned short val = *c++;
        unsigned char a = val &0xff;
        unsigned char b = (val >> 8) &0xff;
        unsigned char color = b;

        if(a != '\b' && a != '\t'){
            if(a == '\n'){
                w->cy++;
                w->cx = 0;
            }else{
                unsigned int fg = w->font.fg_color;
                unsigned int bg = w->font.bg_color;

                if((color&0xf) < 16 && (color&0xf) != 0 )
                    fg = color_table[(color&0xf)];

		        drawchar( a, 4 + w->area_x + w->font.x*w->cx, 4 + w->area_y + w->font.y*w->cy, 
		        fg, bg, &w->font, w);
                
		        w->cx++;

                if( b == 0xFF){
                    w->cy++;
                    w->cx = 0;
                }
            }
        }

    }

    w->cx =0;
    w->cy = ((w->area_height-4)/w->font.y) -1;

    return (int)(w->scrolly + 1);
}

int window_putchar( unsigned short int c, unsigned char color, WINDOW *window, FILE *fd)
{

	WINDOW *w = (WINDOW*)window;
    if(!w)return 0;
	
	int limitx = (w->area_width-4)/w->font.x;
	int limity = (w->area_height-4)/w->font.y;
	
	w->font.fg_color = w->text_fg;

    if((w->chcounter*2) >= fd->bsize || (w->chcounter*2) > fd->fsize ){
        w->chcounter = 0;
        //clear
        drawline(w->area_x , w->area_y, w->area_width, w->area_height, w->bg, w);
        w->cx =0;
        w->cy =0;
        w->scrolly = 0;   
    }
		
	if(c == '\b' && (w->cx > 0))
	{
		w->cx--;
		unsigned int fg = w->font.fg_color;
        unsigned int bg = w->font.bg_color;

        if((color&0xf) < 16 && (color&0xf) != 0 )
            fg = color_table[(color&0xf)];

		drawchar( ' ', 4 + w->area_x + w->font.x*w->cx, 4 + w->area_y + w->font.y*w->cy, 
		fg, bg, &w->font, w);
		//w->cx++;
        w->chcounter++;
	} else if(c == '\t') {
		w->cx += 8;
		w->chcounter++;
	} else if(c == '\n') {
	
		w->cx = 0;
		w->cy++;
        w->chcounter++;
		
	} else if(c >= ' ') {

        unsigned int fg = w->font.fg_color;
        unsigned int bg = w->font.bg_color;

        if((color&0xf) < 16 && (color&0xf) != 0 )
            fg = color_table[(color&0xf)];

		drawchar( c, 4 + w->area_x + w->font.x*w->cx, 4 + w->area_y + w->font.y*w->cy, 
		fg, bg, &w->font, w);
		w->cx++;
        w->chcounter++;
	}


    if(w->cx >= limitx &&  c != '\n') {
		w->cx =0;
		w->cy++;
        //return fputc('\n', fd);
        fd->curp = (unsigned char*)(fd->buffer + (fd->off2-2)+ 1);
	    *(unsigned char*)(fd->curp) = 0xFF;
	}
 
    if(w->cy >= limity){
		w->scrolly = window_putchar_scroll(window,fd);
		return c;
		
	}
	return c;
}
