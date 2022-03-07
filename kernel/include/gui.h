#ifndef __GUI_H
#define __GUI_H

extern short font8x16[128*16];
typedef struct _font {
	int x;
	int y;
	unsigned int fg_color;
	unsigned int bg_color;
	unsigned long buf;
}__attribute__ ((packed)) font_t;

typedef struct _mode {

	unsigned int width, height, bpp;
    unsigned int pitch;

}__attribute__ ((packed)) mode_t;

typedef struct _gui
{
	// Default
	unsigned int	pitch;
	unsigned int 	bpp;
	unsigned int 	planes;
    unsigned int	horizontal_resolution;  	//	width;
	unsigned int	vertical_resolution;  		//	hight;
 	unsigned int	pixels_per_scan_line;	
    unsigned int	max_pixel_clock;
	unsigned int 	frame_buffer_size;
	unsigned int	frame_buffer;
	unsigned int	rsv;
	unsigned long long	bank_buffer;
	
	
	// outros
	unsigned long long 	virtual_buffer;
	
	// Extra.area
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	
	//glyph
	unsigned int cursor_x;
	unsigned int cursor_y;
	
	// font
	struct _font font;
	
}__attribute__ ((packed)) gui_t;

typedef struct __WINDOW {
	unsigned int	spinlock;
	unsigned int	rx, ry, bpp, scanline;
	unsigned int 	width, height;
	unsigned int 	pos_x, pos_y;
	
	unsigned int 	area_width, area_height;
	unsigned int 	area_x, area_y;
	
	unsigned int  	fg, bg, text_fg;
	
	unsigned int 	cy, cx;
    unsigned int 	chcounter;
    unsigned int 	scrollx;
    unsigned int 	scrolly;
	font_t		    font;
    unsigned int    terminal;
    unsigned int    terminal_clear;
    unsigned long   terminal_buffer;
    unsigned long   stdin;
    unsigned long   stdout;
    unsigned long   stderr;
	
	unsigned int	style;
	unsigned int	gid;
	char		    rsv[4096 - 156];
	unsigned long 	start;
}__attribute__ ((packed)) WINDOW;


typedef struct _PAINT
{
	unsigned long 	w;
	unsigned long	spinlock;
	struct _PAINT   *next;
}__attribute__ ((packed)) PAINT;


extern struct _gui gui[1];




void initialize_gui(unsigned long pointer);
void put_pixel(long x, long y, unsigned int color);
void put_pixel_buff(long x, long y, unsigned int color,void *buffer);
void refresh_rate();
void clears_creen();
void draw_char_transparent( int x, int y, int ch, unsigned int fg_color, 
							void *buffer,
							struct _font *font);
int glyph(int ch);

void update_gui(unsigned long memory, int width, int height, int pitch, int bpp );


#endif
