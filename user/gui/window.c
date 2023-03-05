#include <window.h>
#include <string.h>
#include <stdlib.h>
#include <pipe.h>
#include <stdio.h>
#include <sys/communication.h>

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

void drawtriagle(int x, int y, int width, int fill, int rgb, WINDOW *w)
{

	int a =0;
	for(int t = width; t > 0; t-- ){
		for(int i=0; i < (t-a); i++){
			put_pixel(x+a, y+i+a, w->width, rgb, &w->start);
		}

		a++;
	}

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


WINDOW *get_window_surface(){
	WINDOW *w = (WINDOW*) (__window);
	return w;
}


WINDOW *init_window(int x, int y, int width, int height, unsigned int fg, unsigned int bg, int style, WINDOW *window){
	// Limpar de acordo ao limit de buffer
	WINDOW *w = (WINDOW*) window;

	if(width == -1 || width > w->rx) w->width = w->rx;
    else w->width = width;

    if(height == -1 || height > w->ry) w->height = w->ry;
    else w->height = height;

	w->pos_x = x;
	w->pos_y = y;

	w->fg = fg;
	w->bg = bg;

	//init font
	w->font.x = 8;
	w->font.y = 16;
	w->font.fg_color = w->fg;
	w->font.bg_color = w->bg;

    char *tmp = (char*) malloc(FONT_DATA_SIZE);
	memcpy(tmp, font8x16, FONT_DATA_SIZE);
    w->font.buf = (unsigned long)tmp; 

	w->cy = w->cx = 0;
	w->style = style;

	drawline(0, 0, w->width, w->height, w->bg, w);
	drawrect(0, 0, w->width, w->height, 0/*w->fg*/, w);

	if(style == WINDOW_STYLE_DEFAULT){
		w->area_width = w->width - 2;
		w->area_height = w->height - (48+16+2);
		w->area_x = 1;
		w->area_y = 48+1;

		// barra de titulo
		drawline(1, 1, w->width-2, 24, w->fg, w);
		drawline(1, 1 + (24-1), w->width-2, 1, 0xFFFFFF, w);
		// barra de menu
		drawline(1, 1+24, w->width -2, 24, 0xe0e0e0, w);
		drawline(1, 1+48, w->width -2, 1, w->fg, w);

		// button status
		int _x = w->width -4 - 18;
		int _y = 3;
		create_button("X", _x, _y, 18, 18, 0, 0xe0e0e0, w, 1);
		create_button("M", _x-20, _y, 18, 18, 0, 0xe0e0e0, w, 2);
		create_button("-", _x-40, _y, 18, 18, 0, 0xe0e0e0, w, 3);

	}else if(style == WINDOW_STYLE_FLAT){
		w->area_width = w->width;
		w->area_height = w->height;
		w->area_x = 0;
		w->area_y = 0;
	}

	return w;
}

WINDOW *create_new_window(WINDOW *window, int x, int y){
	WINDOW *w = (WINDOW*) malloc(x*4*y);
	memset(w,0,0x1000);
	w->rx = x;
	w->ry = y;
	w->bpp = window->bpp;
	w->scanline = window->scanline;

	return w;
}

void __wcl(WINDOW *w, struct con *con) {
    unsigned long pid = 0;
    // getpid()
    __asm__ __volatile__("int $0x72":"=a"(pid):"d"(1),"c"(0));
	
	con->req.type = COMMUN_TYPE_WINDOW_REGISTER;
	con->req.pid = pid;
	unsigned long *message = (unsigned long*)((unsigned long)&con->req.message);
	*message = (unsigned long)w;
	// enviar para window server
	con_sendto(con);
	w->visibility = 1; 
}

unsigned int rgb(int r, int g, int b){
	return (r<<16 &0xff0000)|(g<<8 &0xff00)|(b &0xff);
}

void create_button(char *text, int x, int y, int width, int height, unsigned int fg, unsigned int bg, WINDOW *w, int id){
	
	drawline(x, y, width, height, bg, w);

	drawline(x+1, y, width-1, 1, 0, w);
	drawline(x, y+1, 1, height-2,0, w);
	drawline(x+width, y+1, 1, height-2,0, w);
	drawline(x+1, y+height, width-1,1, 0, w);
	//drawrect(x, y, width, height, fg, w);

	int cx = x + (width - (strlen(text)*8))/2;
	int cy = y + (height-12)/2;
	if(cx < 0) cx = 0;
	if(cy < 0) cy = 0;

	drawstring_trans(text, cx, cy, fg, 0, &w->font,w);

	WINDOW *new = (WINDOW*)malloc(0x1000);
	memset(new, 0, 0x1000);

	new->id = id;
	new->pos_x = x;
	new->pos_y = y;
	new->width = width;
	new->height = height;
	new->fg = fg;
	new->bg = bg;
	new->type = WINDOW_TYPE_BUTTON;
	int len = strlen(text);
	if(len > 256) len = 256;
	memcpy(new->text, text, len);

	WINDOW *tmp = w;
	while(tmp->next) tmp = tmp->next;
	tmp->next = new;
}

void handler_button(WINDOW *w, WINDOW *btn, int flag){
	char *text = (char *) btn->text;
	int x = btn->pos_x;
	int y = btn->pos_y;
	int width = btn->width;
	int height = btn->height;
	unsigned int fg = btn->fg;
	unsigned int bg = btn->bg;

	if(flag == 1){ //no default
		fg = 0xffffff;
		bg = 0x008f00;
	}else if(flag == 2){ //no default
		fg = 0xffffff;
		bg = 0x8f0000;
	}
	

	drawline(x, y, width, height, bg, w);

	drawline(x+1, y, width-1, 1, 0, w);
	drawline(x, y+1, 1, height-2,0, w);
	drawline(x+width, y+1, 1, height-2,0, w);
	drawline(x+1, y+height, width-1,1, 0, w);
	//drawrect(x, y, width, height, fg, w);

	int cx = x + (width - (strlen(text)*8))/2;
	int cy = y + (height-12)/2;
	if(cx < 0) cx = 0;
	if(cy < 0) cy = 0;

	drawstring_trans(text, cx, cy, fg, 0, &w->font,w);
}

void create_textbox(char *text, int x, int y, int width, int height, unsigned int fg, unsigned int bg, WINDOW *w, int id){
	
	x += w->area_x;
	y += w->area_y;

	drawline(x, y, width, height, bg, w);
	drawrect(x, y, width, height, 0, w);

	WINDOW *new = (WINDOW*)malloc(0x1000);
	memset(new, 0, 0x1000);

	new->id = id;
	new->pos_x = x;
	new->pos_y = y;
	new->width = width;
	new->height = height;
	new->fg = fg;
	new->bg = bg;
	new->type = WINDOW_TYPE_TEXTBOX;
	int len = strlen(text);
	if(len > 256) len = 256;
	memcpy(new->text, text, len);

	WINDOW *tmp = w;
	while(tmp->next) tmp = tmp->next;
	tmp->next = new;
}

void handler_textbox(WINDOW *w, WINDOW *txtbox, int flag){
	char *text = (char *) txtbox->text;
	int x = txtbox->pos_x;
	int y = txtbox->pos_y;
	int width = txtbox->width;
	int height = txtbox->height;
	unsigned int fg = txtbox->fg;
	unsigned int bg = txtbox->bg;

	unsigned int color = 0x0000ff;
	if(flag == 0)color = 0;

	//drawline(x, y, width, height, bg, w);
	drawrect(x, y, width, height, color, w);
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

    w->stdin = (unsigned long)stdin;
    w->stdout = (unsigned long)stdout;
    w->stderr = (unsigned long)stderr;

	return (w);
}


void wcl(WINDOW *w) {

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

	w->visibility = 1; 
}

void update_window(WINDOW *w)
{
	//window(const char *title,int x, int y, int width, int height )

}

void __window_clear(WINDOW *w)
{
    w->terminal_clear = 1;
}

