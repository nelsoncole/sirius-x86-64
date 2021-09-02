#include <window.h>
#define X 12
#define Y 12

unsigned int A__[12*12]={
0,0,0,0,0,0xE9E9E9,0xFFFFFF,0x7E7E7E,0,0,0,0,
0,0,0,0,0x757575,0xFFFFFF,0xE9E9E9,0xCBCBCB,0,0,0,0,
0,0,0,0,0xC4C4C4,0xF3F3F3,0xBDBDBD,0xF9F9F9,0x3B3B3B,0,0,0,
0,0,0,0x1C1C1C,0xF4F4F4,0xD1D1D1,0x7E7E7E,0xFFFFFF,0xA6A6A6,0,0,0,
0,0,0,0x919191,0xFFFFFF,0x919191,0,0xEDEDED,0xE4E4E4,0,0,0,
0,0,0,0xD3D3D3,0xF0F0F0,0x161616,0,0xB2B2B2,0xFFFFFF,0x707070,0,0,
0,0,0x404040,0xFBFBFB,0xB9B9B9,0,0,0x494949,0xFBFBFB,0xC4C4C4,0,0,
0,0,0xA7A7A7,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xFFFFFF,0xF6F6F6,0,0,
0,0,0xE1E1E1,0xD8D8D8,0,0,0,0,0x7A7A7A,0xFFFFFF,0x9C9C9C,0,  
0,0x606060,0xFFFFFF,0xA4A4A4,0,0,0,0,0,0xEFEFEF,0xDDDDDD,0,
0,0xB9B9B9,0xFEFEFE,0x4D4D4D,0,0,0,0,0,0xC1C1C1,0xFEFEFE,0x626262,
0,0xEDEDED,0xE0E0E0,0,0,0,0,0,0,0x7C7C7C,0xFFFFFF,0xBDBDBD,
};


/*
   00000000100000000
   00000001110000000
   00000011011000000
   00000110001100000
   00001100000110000
   00011000000011000
   00111111111111100
   01100000000000110
   01100000000000110	
   11000000000000011*/
   
   
#define GX_ALPHA(c, a) (((c) & (~0xFF)) | ((a) & 0xFF))
#define GX_RGBA(r, g, b, a) (((r) << 3*8) | ((g) << 2*8) | ((b) << 1*8) | ((a) << 0*8))
#define GX_RGB(r, g, b) (((r) << 3*8) | ((g) << 2*8) | ((b) << 1*8))


void put(int c, int x, int y, WINDOW *w)
{
	for(int t=0; t < Y; t ++)
	for(int i=0; i < X; i ++) {
	
	
		unsigned int color = A__[t*X+i];
		if(color)
		put_pixel(x+i, y+t, w->width,color, &w->start);
		
		
	}

}
