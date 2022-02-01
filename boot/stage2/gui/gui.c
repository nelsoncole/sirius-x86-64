#include <gui.h>
#include <stdlib.h>
#include <string.h>
struct _gui *gui;

extern void puts(const char *s);
void initialize_gui()
{
	gui = (struct _gui*) malloc(sizeof(gui_t));
	memset(gui,0,sizeof(gui_t));
	
	// BIOS VESA Video and Audio
	unsigned char *vbe = (unsigned char*)0x30200;
	unsigned char *vbe_info_mode = vbe + 0x200;

	// GUI
	gui->pitch 		= (*(unsigned short*)(vbe_info_mode + 16));
	gui->bpp 			= (*(unsigned char*)(vbe_info_mode  + 25));
	gui->planes		= (*(unsigned char*)(vbe_info_mode  + 24));
	gui->horizontal_resolution	= (*(unsigned short*)(vbe_info_mode + 18));  	//	width;
	gui->vertical_resolution	= (*(unsigned short*)(vbe_info_mode + 20)); 	//	hight;
	gui->pixels_per_scan_line	= gui->horizontal_resolution;//GraphicsOutputProtocol->Mode->Info->PixelsPerScanLine;	
	gui->max_pixel_clock	= 0;
	gui->frame_buffer	= (unsigned int) (*(unsigned int*)(vbe_info_mode + 40));
	gui->frame_buffer_size	= (*(unsigned short*)(vbe + 18));
	gui->bank_buffer	= (unsigned int) malloc(0x800000); // 8MB
	
	
	//
	gui->virtual_buffer = gui->frame_buffer;
			
	// Extra.area		
	gui->x 	= 0;
	gui->y 	= 0;
	gui->width 	= gui->horizontal_resolution;
	gui->height 	= gui->vertical_resolution;
	
	gui->pixels_per_scan_line = gui->horizontal_resolution;


	gui->font.x = 8;
	gui->font.y = 16;
	gui->font.fg_color = 0xffffff;
	gui->font.bg_color = 0;
	gui->font.buf = (unsigned long)font8x16;


}


void put_pixel(long x, long y, unsigned int color)
{
	
	*(unsigned int*)((unsigned int*)gui->virtual_buffer + (gui->pixels_per_scan_line * y) + x) = color;
	
}

void put_pixel_buff(long x, long y, unsigned int color,void *buffer)
{
	
	unsigned int *buf = (unsigned int*)buffer;

	if((x >= gui->horizontal_resolution) || (gui->vertical_resolution) ) return;	

	*(unsigned int*)((unsigned int*)buf + (gui->pixels_per_scan_line * y) + x) = color;
	
}

void refresh_rate() 
{ 
	
	memcpy((void*)gui->virtual_buffer, (void*)gui->bank_buffer,\
	gui->width * (gui->height) * (gui->bpp/8));
	
}


void clears_creen() 
{
	memset((void*)gui->virtual_buffer/*G->BankBuffer*/,0,gui->width * (gui->height) * (gui->bpp/8));
	gui->cursor_x = gui->cursor_y = 0;
}


void draw_char_transparent( int x, int y, int ch, unsigned int fg_color, 
							void *buffer,
							struct _font *font)
{
	unsigned short f = 0;
	const unsigned short *font_data = (unsigned short*) font->buf;
   	int cx, cy;
	unsigned short mask;
    
   	for(cy=0;cy < font->y ;cy++){
		mask = 1;
        	f = font_data[(ch *font->y) + cy];
		
                for(cx = font->x -1 ;cx >= 0;cx--){ 
                       if(f&mask){
				//put_pixel_buff(x + cx,y + cy,fg_color,buffer);
				put_pixel(x + cx,y + cy,fg_color);
			}
			mask += mask;
                  
          }
        
    }

}


int glyph(int ch)
{
	
	
	if(ch == '\n') {
	
		gui->cursor_x = 0;
		gui->cursor_y++;
		return ch;
	
	}
	
	if(ch < ' ') return ch;
	
	
	if(gui->cursor_y*gui->font.y >= gui->height-16) {
	
		gui->cursor_y = 0;
	}
	
	if(gui->cursor_x*gui->font.x >= gui->width) {
	
		gui->cursor_x = 0;
		gui->cursor_y++;
	}

	draw_char_transparent(gui->cursor_x*gui->font.x, gui->cursor_y*gui->font.y, ch, gui->font.fg_color, (void*)gui->bank_buffer,&gui->font);
	
	gui->cursor_x++;
	
	return ch;
}
