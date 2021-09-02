#ifndef __GUI_H
#define __GUI_H


#include <window.h>


extern short font8x16[128*16];


void put_pixel(long x, long y, unsigned int scanline,unsigned int color,void *buffer);
/*void draw_char_transparent( int x, int y, unsigned int scanline, int ch, void *buffer,
							struct _font *font);*/

#endif
