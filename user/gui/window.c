#include <window.h>
#include <string.h>
#include <stdlib.h>

void drawline(int x1, int y1, int x2, int y2, int rgb, WINDOW *w)
{

	for(int y=0; y < y2; y++ ) {
		for(int x=0; x < x2; x++) {
			put_pixel(x1+x, y1+y, w->width, rgb, &w->start);
		}
	}	
}

void _drawline(int x1, int y1, int x2, int y2, int rgb, WINDOW *w)
{

	for(int y=0; y < y2; y++ ) {
		for(int x=0; x < x2; x++) {
			put_pixel(x1+x, y1+y, w->width, rgb + (y*rgb)*0x08080808, &w->start);
		}
	}	
}

void drawrect(int x, int y, int width, int height, int rgb, WINDOW *w)
{

	if(width <= 0 || height <=  0 ) return;

	for(int i=0; i < width; i++) put_pixel(x+i, y, w->width, rgb, &w->start);
	
	for(int i=0; i < width; i++) put_pixel(x+i, y + height-1, w->width, rgb, &w->start);
	
	
	for(int i=0; i < height; i++) put_pixel(x, y+i, w->width, rgb, &w->start);
	
	for(int i=0; i < height; i++) put_pixel(x + width -1, y+i, w->width, rgb, &w->start);


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
				put_pixel(cx + x,cy + y, w->width, fg, &w->start);
				
			}else{
				put_pixel(cx + x,cy + y, w->width, bg, &w->start);
			}
			mask += mask;
                  
          }
        
    }

}


void drawstring( const char *str, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w)
{
	int x = 0;
	char *tmp_str = (char*)str;
	
	while(*tmp_str) {
	
		drawchar(*tmp_str++,cx + (font->x*x++), cy, fg, bg, font, w);
	}

}


void drawchar_trans( unsigned short int c, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w)
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
				put_pixel(cx + x,cy + y, w->width, fg, &w->start);
				
			}else{
				//put_pixel(cx + x,cy + y, w->width, bg, &w->start);
			}
			mask += mask;
                  
          }
        
    }

}


void drawstring_trans( const char *str, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w)
{
	int x = 0;
	char *tmp_str = (char*)str;
	
	while(*tmp_str) {
	
		drawchar_trans(*tmp_str++,cx + (font->x*x++), cy, fg, bg, font, w);
	}

}


WINDOW *window(const char *title,int x, int y, int width, int height, int fg, int bg, int tfg, int style)
{
	
	WINDOW *w = (WINDOW*) __window;	
	memset((char*)&w->start,0, 0x400000-0x1000);
	
	int menu = 0;
	
	w->fg = fg;
	w->bg = bg;
	w->text_fg = tfg;
	
	//init font
	w->font.x = 8;
	w->font.y = 16;
	w->font.fg_color = w->text_fg;
	w->font.bg_color = bg;

    char *tmp = (char*) malloc(FONT_DATA_SIZE);
	memcpy(tmp, font8x16, FONT_DATA_SIZE);

    w->font.buf = (unsigned long)tmp; 

	w->cy = w->cx = 0;
	
    if(width == -1) w->width = (w->rx - w->rx/4);
    else w->width = width;

    if(height == -1) w->height = (w->ry - w->ry/6);
    else w->height = height;

	w->pos_x = x;
	w->pos_y = y;
	
	w->style = style;
	
	if(style&0x10) 
		menu = WMENU_SIZE;
	
	w->area_width = w->width -6;

	w->area_height = w->height - (WBART_SIZE + 6 + menu + WROD_SIZE);
	w->area_x = 3;
	w->area_y = WBART_SIZE + 3 + menu;
	
	
	/*
	drawrect(0, 0, w->width, w->height, 0x80808080, w );
	drawrect(1, 1, w->width-2, w->height-2, 0x80808080, w ); */
	
	border(0, w->height, 0, w->width, w);
	
	// barra de title
	drawline(3, 3, w->width-6, WBART_SIZE, w->fg, w);

    
    // separador
	if(menu == 0)
		drawline(3, 3 + (WBART_SIZE-1), w->width-6, 1, 0xC0C0C0, w);
	else
		drawline(3, 3 + (WBART_SIZE-1), w->width-6, 1, 0xFFFFFF, w);
	
	// menu
	if(menu != 0) {
		drawline(3, 3+WBART_SIZE, w->width -6, menu, 0xe0e0e0, w);
		drawline(3, 3+WBART_SIZE + menu, w->width -6, 1, w->fg, w);
	}
	
	//area
	drawline(w->area_x , w->area_y, w->area_width , w->area_height, w->bg, w);
	
	// scroll
	//drawline(w->area_width+10, w->area_y +10, 6,60, 0xe0e0e0, w);
	//drawchar_trans( '^',w->area_width+9, w->area_y + 2, 0xFFFFFF, 0xf0f0f0,&w->font,w);
	
	// title
	int tlen = strlen(title)*8;
	drawstring_trans(title, (w->width/2) - (tlen/2), 6, 0xFFFFFF, 0xf0f0f0, &w->font, w);
	


	// button
	drawrect( w->width - 20, 4, 15, 17, 0, w );
	drawchar_trans( 'X', w->width - 20+4, 6, 0xFFFFFF, 0xf0f0f0,&w->font,w);
	drawchar_trans( '-', w->width - 40, 6, 0xFFFFFF, 0xf0f0f0,&w->font,w);


    // rodapé
    
    drawline( 3, w->height - WROD_SIZE - 3 , w->width-6 , WROD_SIZE, 0xC0C0C0, w);

	return (w);
}

void update(const char *id, WINDOW *w )
{
	
	int _x = w->area_x + w->width - 80;
	int _y = w->area_y + w->height- 28;
	
	int width  = 10*8; 
	int height = 20;
	
	int fg = 0;//-1; 
	int bg = 0xe0e0e0;//0x80808080;
	
	//drawline(_x, _y, width, height, bg, w);
	//drawrect(_x, _y, width, height, bg - 0x10101010, w );
	
	
	// centro
	int len = strlen(id);
	drawstring(id, _x + ( width/2 - (len*8/2)), _y + (height/2 - 8), fg, bg, &w->font, w);
	
}


void wcl(WINDOW *w) {
	__asm__ __volatile__("int $0x72"::"d"(2),"D"(w));
}

void update_window(WINDOW *w)
{

	//window(const char *title,int x, int y, int width, int height )

}

void __window_clear(WINDOW *w)
{
	w->cy = w->cx =0;
	drawline(w->area_x , w->area_y, w->area_width, w->area_height, w->bg, w);
}

int __window_putchar( unsigned short int c)
{

	WINDOW *w = (WINDOW*) __window;
	
	//w->font.buf = (unsigned long)font8x16;

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
		__window_clear(w);
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


void color(int rgb){

	WINDOW *w = (WINDOW*) __window;

	w->text_fg = rgb;
}


void __window_puts(char* s)
{
	if(!s)return;
	 
	char *tmp_s = s; 
	while(*tmp_s)__window_putchar(*tmp_s++);
 }
