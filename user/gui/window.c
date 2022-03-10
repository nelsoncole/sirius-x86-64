#include <window.h>
#include <string.h>
#include <stdlib.h>
#include <pipe.h>

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

/*
 * 0x10 MENU
 * 0x20 Scroll
 */

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
	
    if(width == -1 || width > w->rx) w->width = w->rx;
    else w->width = width;

    if(height == -1 || height > w->ry) w->height = w->ry;
    else w->height = height;

	w->pos_x = x;
	w->pos_y = y;
	
	w->style = style;
	
	if(style&0x10) 
		menu = WMENU_SIZE;
	
	w->area_width = w->width -6;
    
    if(style&0x20){
        w->area_width -= SCROLL_SIZE;
    }

	w->area_height = w->height - (WBART_SIZE + 6 + menu + WROD_SIZE);
	w->area_x = 3;
	w->area_y = WBART_SIZE + 3 + menu;
	
	
	/*
	drawrect(0, 0, w->width, w->height, 0x808080, w );
	drawrect(1, 1, w->width-2, w->height-2, 0x808080, w ); */
	
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


void wcl(WINDOW *w) {

    // enviar para compositor 
    // TODO esta operação será feita no window server
	// __asm__ __volatile__("int $0x72"::"d"(2),"D"(w));

    // enviar para window server
    unsigned long id = 0;
    __asm__ __volatile__("int $0x72":"=a"(id):"d"(14));
    unsigned long addr = 0;
    // get pipe launcher
    __asm__ __volatile__("int $0x72":"=a"(addr):"d"(12));
    FILE *pipe_launcher = (FILE *)addr;
    addr = (unsigned long) w;

    short pipe_ptr[8] = {0,0,0,0,0,0,0,0};

    __pipe_t *pipe = (__pipe_t *) ((unsigned long *)&pipe_ptr);

	pipe->id = PIPE_WINDOW_REGISTER;
    pipe->r1 = id &0xffff;
    pipe->r2 = id >> 16 &0xffff;
    pipe->r3 = id >> 32 &0xffff; //id >> 48 &0xffff; */
    pipe->r4 = addr;
	pipe_write ( pipe, pipe_launcher ); 
}

void update_window(WINDOW *w)
{
	//window(const char *title,int x, int y, int width, int height )

}

void __window_clear(WINDOW *w)
{
    w->terminal_clear = 1;
}

