#include <stdio.h>
#include <gui.h>

static unsigned int color_table[] = {0x000000,0x0000FF,0x00FF00,0x00FFFF,0xFF0000,0xAA00AA,\
0xAA5500,0x808080,0x555555,0x5555FF,0x55FF55,0x55FFFF,0xFF5555,0xFF55FF,0xFFFF00,0xFFFFFF};

void put_pixel_sirius(long x, long y, unsigned int scanline,unsigned int color, WINDOW *window)
{
	WINDOW *w = (WINDOW*)window;
    if(!window) return;
	
	if(x >= scanline || y >= w->ry) return;
	
    unsigned long addr = (unsigned long)((unsigned long*)&w->start);
	unsigned int *buf = (unsigned int*)addr;

	*(unsigned int*)((unsigned int*)buf + (scanline * y) + x) = color;
	
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
				put_pixel_sirius(cx + x,cy + y, w->width, fg, w);
				
			}else{
				put_pixel_sirius(cx + x,cy + y, w->width, bg, w);
			}
			mask += mask;
                  
          }
        
    }

}

void drawline(int x1, int y1, int x2, int y2, int rgb, WINDOW *w)
{

	for(int y=0; y < y2; y++ ) {
		for(int x=0; x < x2; x++) {
			put_pixel_sirius(x1+x, y1+y, w->width, rgb, w);
		}
	}	
}

static int window_putchar_scroll(WINDOW *window, int foco, int op){
    WINDOW *w = (WINDOW*)window;
    if(!w)return 0;

    XCOMPONENT *com = (XCOMPONENT*)w->component[foco];
    unsigned int x = com->x;
    unsigned int y = com->y;

    unsigned short *buffer = (unsigned short *)com->addr;

    // clear
    drawline(x , y, com->width, com->height, com->bg, w);

    // eliminar linha...
    unsigned short *c = buffer;
    int i = com->offset;
    int scrolly = com->scrolly.x;

    if(op == 0)
    for(; i > 0; --i){
        unsigned short val = *c++;
        unsigned char ch = val &0xff;

        if( ch == '\n'){
            if(!scrolly)
                break;

            scrolly--;
        }
    }

    if(op == 1)
    {
        i = com->scrolly.index;
        c = (unsigned short *) com->scrolly.addr;
        //i += com->offset - i;
    }
    else
    {
        // record
        com->scrolly.index = i;
        com->scrolly.addr = (unsigned long)c;
    }


    com->cx =0;
    com->cy =0;

    if(i <= 0 && op == 0){ return 0;}

    for( ;i > 1; --i){

        unsigned short val = *c++;
        unsigned char ch = val &0xff;
        unsigned char color = (val >> 8) &0xff;

        if(ch != '\b' && ch != '\t'){
            if(ch == '\n'){
                com->cy++;
                com->cx = 0;
            }else{
                unsigned int fg = com->font.fg_color;
                unsigned int bg = com->font.bg_color;

                if((color&0xf) < 16 && (color&0xf) != 0 )
                    fg = color_table[(color&0xf)];

                if(((color >> 4 )&0xf) < 16 && ((color >> 4 )&0xf) != 0 )
                    bg = color_table[((color >> 4)&0xf)];

		        drawchar( ch, 4 + x + com->font.x*com->cx, 4 + y + com->font.y*com->cy, 
		        fg, bg, &com->font, w);
                
		        com->cx++;

            }
        }

    }

    if(op == 1)
    {
        return 0;
    }

    com->cx =0;
    com->cy = ((com->height-0)/com->font.y) -1;

    // scroll
    int ty = (com->scrolly.x + 1);
    int limity = (com->height-4)/com->font.y;
    limity = limity*0.9;
    if(ty > limity){
        ty = limity;
    }

    com->scrolly.size = ty = 16*ty;
    drawline(x + com->width + 4, y+2, 8, com->height-4, com->bg, w);
    drawline(x + com->width + 4, y+2+ty, 8, (com->height-4)-ty, 0xe0e0e0, w);

    com->scrolly.width = (com->height-4)-ty;

    return (int)(com->scrolly.x + 1);
}

static void window_putchar_scroll2(WINDOW *window, int foco){
    WINDOW *w = (WINDOW*)window;
    if(!w)return;

    XCOMPONENT *com = (XCOMPONENT*)w->component[foco];
    unsigned int x = com->x;
    unsigned int y = com->y;
    int limity = (com->height - 4)/com->font.y;
    unsigned short *buffer = (unsigned short *)com->addr;

    // clear
    drawline(x , y, com->width, com->height, com->bg, w);

    // eliminar linha...
    unsigned short *c = buffer;
    int i = com->offset;
    int scrolly = com->scrolly.y;
    
    if(scrolly > 0)
    for(; i > 0; --i){
        unsigned short val = *c++;
        unsigned char ch = val &0xff;

        if( ch == '\n')
        {
            scrolly--;
            if(!scrolly)
                break;
        }
    }

    int cx =0;
    int cy =0;

    if(i <= 0){ return;}

    for( ;i > 1; --i){

        if(cy >= limity )break;
        unsigned short val = *c++;
        unsigned char ch = val &0xff;
        unsigned char color = (val >> 8) &0xff;

        if(ch != '\b' && ch != '\t'){
            if(ch == '\n'){
                cy++;
                cx = 0;
            }else{
                unsigned int fg = com->font.fg_color;
                unsigned int bg = com->font.bg_color;

                if((color&0xf) < 16 && (color&0xf) != 0 )
                    fg = color_table[(color&0xf)];

                if(((color >> 4 )&0xf) < 16 && ((color >> 4 )&0xf) != 0 )
                    bg = color_table[((color >> 4)&0xf)];

		        drawchar( ch, 4 + x + com->font.x*cx, 4 + y + com->font.y*cy, 
		        fg, bg, &com->font, w);
                
		        cx++;

            }
        }

    }

    com->scrolly.flag = 1;
}

int window_putchar( unsigned short int c, unsigned char color, WINDOW *window, int foco)
{

	WINDOW *w = (WINDOW*)window;
    if(!w)return 0;

    XCOMPONENT *com = (XCOMPONENT*)w->component[foco];

    unsigned int x = com->x;
    unsigned int y = com->y; 

    unsigned short *buf = (unsigned short *)com->addr;
	
	int limitx = (com->width - 4)/com->font.x;
	int limity = (com->height - 4)/com->font.y;
	
	com->font.fg_color = com->fg;

    if((com->offset*2) >= 0x10000 || com->clear == 1){
        //clear
        com->clear = 0;
        drawline(x , y, com->width, com->height, com->bg, w);
        com->cx =0;
        com->cy =0;
        com->scrolly.x = 0;
        com->offset = 0;

        //com->scrolly.flag = 1;
    
        if(c == '\n') return c;
    }


    if(com->scrolly.flag != 0)
    {
        com->scrolly.flag = 0;
        window_putchar_scroll(window, foco, 1);
        //return c;
    }
		
	if(c == '\b' && (com->cx > 0))
	{
		com->cx--;
		unsigned int fg = com->font.fg_color;
        unsigned int bg = com->font.bg_color;

        if((color&0xf) < 16 && (color&0xf) != 0 )
            fg = color_table[(color&0xf)];

        com->offset--;
        com->scrolly.index--;
        buf[com->offset] =' ' | color << 8;
		drawchar( ' ', 4 + x + com->font.x*com->cx, 4 + y + com->font.y*com->cy, 
		fg, bg, &com->font, w);
		//com->cx++;
        
	} else if(c == '\n') {
	
		com->cx = 0;
		com->cy++;
        buf[com->offset] ='\n' | color << 8;
        com->offset++;
        com->scrolly.index++;
		
	} else if(c == '\t') {

        unsigned int fg = com->font.fg_color;
        unsigned int bg = com->font.bg_color;

        if((color&0xf) < 16 && (color&0xf) != 0 )
            bg = color_table[(color&0xf)];

        if(((color >> 4 )&0xf) < 16 && ((color >> 4 )&0xf) != 0 )
            bg = color_table[((color >> 4)&0xf)];

        buf[com->offset] = c | color << 8;
        com->offset++;
        com->scrolly.index++;
        for(int i=0; i < 8; i++) {
		    drawchar(' ', 4 + x + com->font.x*com->cx, 4 + y + com->font.y*com->cy, fg, bg, &com->font, w);
		    com->cx++;

            if(com->cx >= limitx) {
		        com->cx =0;
		        com->cy++;
                buf[com->offset] = '\n' | 0xFF << 8;
                com->offset++;
                com->scrolly.index++;
	        }
 
            if(com->cy >= limity){
		        com->scrolly.x = window_putchar_scroll(window, foco, 0);	
	        }
        }

	} else if(c >= ' ') {

        unsigned int fg = com->font.fg_color;
        unsigned int bg = com->font.bg_color;

        if((color&0xf) < 16 && (color&0xf) != 0 )
            fg = color_table[(color&0xf)];

        if(((color >> 4 )&0xf) < 16 && ((color >> 4 )&0xf) != 0 )
            bg = color_table[((color >> 4)&0xf)];

		drawchar( c, 4 + x + com->font.x*com->cx, 4 + y + com->font.y*com->cy, 
		fg, bg, &com->font, w);
		com->cx++;

        buf[com->offset] = c | color << 8;
        com->offset++;
        com->scrolly.index++;

	}else { com->offset++; com->scrolly.index++; }


    if(com->cx >= limitx &&  c != '\n') {
		com->cx =0;
		com->cy++;
        buf[com->offset] = '\n' | 0xFF << 8;
        com->offset++;
        com->scrolly.index++;
	}
 
    if(com->cy >= limity){
		com->scrolly.x = window_putchar_scroll(window, foco, 0);
	}
	return c;
}


static int zy;
void terminal_mouse_click(int x, int y, XCOMPONENT *com, WINDOW *w )
{
    int x1 = com->x + com->width;
    int y1 = com->y;
	int x2 = 16;
	int y2 = com->height;

    if( /*x > x1 &&*/ y > y1 /*&& x < (x1+x2)*/ && y < (y1 + y2) )
    {
        if(zy == y) return;
        zy = y;
        drawline(com->x + com->width + 4, com->y+2, 8, com->height-4, com->bg, w);

        int limit = (com->height-4)/com->font.y;
        limit = limit*0.9;
        //limit = 16*limit;

        com->scrolly.y = (y-com->y)/limit;

        if((y+com->scrolly.width) > ((com->height-4)+com->y))
            y -= (y+com->scrolly.width) - ((com->height-4)+com->y);

        drawline(com->x + com->width + 4, y+2, 8, com->scrolly.width, 0xe0e0e0, w);

        window_putchar_scroll2(w, w->foco);   
    }

}
