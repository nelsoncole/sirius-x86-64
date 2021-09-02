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
	unsigned int	rsv1;
	unsigned int	bank_buffer;
	unsigned int	rsv2; 
	
	//
	unsigned int 	virtual_buffer;
	unsigned int	rsv3;
	
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

extern struct _gui *gui;



void initialize_gui();
void put_pixel(long x, long y, unsigned int color);
void put_pixel_buff(long x, long y, unsigned int color,void *buffer);
void refresh_rate();
void clears_creen();
void draw_char_transparent( int x, int y, int ch, unsigned int fg_color, 
							void *buffer,
							struct _font *font);
int glyph(int ch);


#endif
