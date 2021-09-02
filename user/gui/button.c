#include <window.h>
#include <data.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void button(WINDOW *w)
{
	int bBColor1 = 0xFFFFFF;
	int bBColor2 = 0x606060;
	int bBColor2_light = 0xe0e0e0;

	int x1 = w->area_x + 40;
	int y1 = w->area_y + 100;
	int x2 = 100;
	int y2 = 50; 

	// cima
	drawrect(x1+1, y1, x2-2, 1, 0, w);
	drawrect(x1+1, y1+1, x2-2, 1, bBColor1, w);
	drawrect(x1+2, y1+2, x2-4, 1, bBColor1, w);
	
	//esqerda
	drawrect(x1, y1+1, 1, y2-2, 0, w);
	drawrect(x1+1, y1+1, 1, y2-2, bBColor1, w);
	drawrect(x1+2, y1+2, 1, y2-4, bBColor1, w);
	
	//direita
	drawrect((x1 + x2) -3, y1+2, 1, y2-4, bBColor2_light, w);
	drawrect((x1 + x2) -2, y1+1, 1, y2-2, bBColor2, w);
	drawrect((x1 + x2) -1, y1+1, 1, y2-2, 0, w);
	
	// baixo
	drawrect(x1+2, (y1 + y2) -3, x2-4, 1, bBColor2_light, w);
	drawrect(x1+1, (y1 + y2) -2, x2-2, 1, bBColor2, w);
	drawrect(x1+1, (y1 + y2) -1, x2-2, 1, 0, w);

}
