#include <window.h>


int dialogbox(WINDOW *w, void *buf)
{

	char *s = (char *)buf;
	
	// desenhar janela
	
	int x1 = (w->width/2) - 150;  
	int y1 = (w->height/2) - 50;
	int x2 = 300;
	int y2 = 100;
	
	drawline(x1+2, y1+2, x2-4, y2-4, 0x101020, w);
	drawrect(x1+1, y1+1, x2-2, y2-2, 0x80808080,w);
	drawrect(x1, y1, x2, y2, 0x80808080,w);
	
	
	// title
	drawstring_trans( "File name", x1 + 10, y1 + 8, -1, 0, &w->font, w);
	
	// caixa de texto
	x1 += 8;
	y1 += 30;
	x2 -= 16;
	y2 -= 74;
	
	drawline(x1+2, y1+2, x2-4, y2-4, -1, w);
	drawrect(x1+1, y1+1, x2-2, y2-2, 0x80808080,w);
	drawrect(x1, y1, x2, y2, 0x80808080,w);
	
	
	unsigned short int  c;
	int i = 0;
	for( ; ;) {
		c = getkey();
		
		if (c == '\n') 
			break;
		else if(c != 0 && i > 0 && c == '\b'){
			i --;
			c = ' ';
			drawchar( c, x1 + 4 + (i*8), y1 + 4 , 0, -1, &w->font, w);
			
			s--;
		}
		else if (c != 0) {
			drawchar( c, x1 + 4 + (i*8), y1 + 4 , 0, -1, &w->font, w);
			i ++;
			
			*s++ = c;
		}
	}
	
	*s = 0;
	
	
	while(w->spinlock) __asm__ __volatile__("pause;");
	w->spinlock++;
		
	update_objs(w);
		
	w->spinlock = 0;
		
	return 0; // sucesso
}
