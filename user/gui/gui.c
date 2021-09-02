#include <gui.h>



void put_pixel(long x, long y, unsigned int scanline,unsigned int color,void *buffer)
{
	WINDOW *w = (WINDOW*) (__window);
	
	if(x >= scanline || y >= w->ry) return;
	
	unsigned int *buf = (unsigned int*)buffer;

	*(unsigned int*)((unsigned int*)buf + (scanline * y) + x) = color;
	
}


void draw_char_transparent( int x, int y, unsigned int scanline, int ch, void *buffer,
							struct _font *font)
{
	unsigned short f = 0;
	const unsigned short *font_data = (unsigned short*) font->buf;
   	int cx, cy;
	unsigned short mask;
	unsigned int color = font->fg_color;
	
	if(ch == '\b') { 
	
		color = 0;
	}
    
   	for(cy=0;cy < font->y ;cy++){
		mask = 1;
        	f = font_data[(ch *font->y) + cy];
		
                for(cx = font->x -1 ;cx >= 0;cx--){ 
                       if(f&mask){
				put_pixel(x + cx,y + cy, scanline, color,buffer);
				
			}
			mask += mask;
                  
          }
        
    }

}
