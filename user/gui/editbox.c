#include <window.h>
#include <stdlib.h>
#include <string.h>


#define ESC 0x1b
#define CONTROL '['
#define RIGHT 'C'
#define LEFT 'D'


static void editbox_refresh(HANDLE_T *obj);

HANDLE_T *editbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id) {
	if(w == 0) return 0;
	HANDLE_T *obj = malloc(sizeof(HANDLE_T));
	if(obj == 0) return 0;

	memset(obj, 0, sizeof(HANDLE_T) );
	
	obj->w  = (unsigned long) w;
	obj->id = id;
	obj->x  = w->area_x + x;
	obj->y  = w->area_y + y;
	
	obj->type = TYPE_EDITBOX;
	
	if(width >= w->area_width) obj->width  = w->area_width - x;
	else obj->width  = width;
	if(height >= w->area_height) obj->height  = w->area_height - y;
	else obj->height = height;
	
	obj->fg = fg;
	obj->bg = bg;
	obj->len = 0x2000; // 8KiB
	obj->addr = (unsigned long) malloc(obj->len);
	if(obj->addr != 0) memset((char*)obj->addr, 0, obj->len );
	
	//area
	drawline( obj->x ,obj->y, obj->width, obj->height , obj->bg, w);
	drawrect(obj->x, obj->y, obj->width, obj->height, 0x808080, w);
	
	return obj;
}


void update_editbox(HANDLE_T *obj)
{
	WINDOW *w = (WINDOW*)obj->w;

	//area
	drawline( obj->x ,obj->y, obj->width, obj->height , obj->bg, w);
	drawrect(obj->x, obj->y, obj->width, obj->height, 0x808080, w);

	editbox_refresh(obj);
	
	//printf("EDITBOX\n");
}


int m_edit(HANDLE_T *obj) {
	int c;
	if(!obj) return -1;
	
	//WINDOW *w = (WINDOW *) obj->w;
	
	c = getkey();
	if(c == ESC) {
		c = getkey();
		if(c == CONTROL) {
			c = getkey();
			
			switch(c) {
				case RIGHT:
					if(obj->offset < obj->bytes) obj->offset ++;
				break;
				case LEFT:
					if(obj->offset > 0) obj->offset --;
				break;
			}
			
			editbox_refresh(obj);
		
		}
		
		return 0;
	}
	

	
	char *s = (char*) (obj->addr + obj->offset);
	if(c != 0 && obj->offset < obj->len ) { 
	
		obj->flag = 0;
		
		if(c == '\b')
		{
			if(obj->bytes > 0 && obj->offset > 0 ) 
			{	s --;
				memmove(s, s + 1, obj->bytes - obj->offset);
				obj->offset --;
				obj->bytes --;
				
				
			} else {
			
				obj->flag = 1;
			}
			
		}else {
		
			if(obj->offset != obj->bytes) {
			
				memmove(s + 1, s, obj->bytes - obj->offset);
				
			}
			
			*s = c &0xff; 
			
			obj->offset ++;
			obj->bytes ++;
			
			// TODO so e so se estiver no fim
			obj->flag = 2;
		}
		
		editbox_refresh(obj);
		
	}
	
	if(!(obj->flag&0x1)) {
	
		editbox_refresh(obj);
		obj->flag |= 1;
	}
	
	return 0;
}


static void editbox_refresh(HANDLE_T *obj)
{
	WINDOW *w = (WINDOW *) obj->w;
	char *s = (char*) obj->addr;
	int count = 0;
	unsigned short int c = 0;
	if(obj->flag == 1) { 
		obj->flag = 0;
		return;
	}
	
	/*if(obj->flag == 2) {
	
		s = s + obj->bytes - 1;
		
		c = *s;
		
		if(c == '\n') c = ' ';

		if(obj->cy >= obj->height || obj->cx >= obj->width ) {
			obj->cy += 16;
			obj->cx = 0; 
		}
		drawchar( c, obj->x + obj->cx, obj->y + obj->cy, obj->fg, obj->bg, &w->font, w);
		
		obj->cx += 8;
		
		obj->flag = 0;
		return;
	}*/
	
	for(int y = 4; y < (obj->height - 16); y += 16 ) {
		int fill = 0;
		
		for(int x = 4; x < (obj->width - 8); x += 8 ){
		
			if (fill == 0) {
		
				if(count < obj->bytes) 
				{ 
					c = *s ++;
					
				}
				else c = ' ';
			
				if(c == '\n') {
					c = ' '; 
					fill = 1;
				}
				
				// salvar cursor
				if(count == obj->offset) {
					obj->cx = x;
					obj->cy = y;
				}
				
				count ++;
			}
			
			if(c != '\0') {
				drawchar( c, obj->x + x, obj->y + y, obj->fg, obj->bg, &w->font, w);
			}
			
		}
	}
	//update cursor
	drawchar_trans('_', obj->x + obj->cx, obj->y + obj->cy, obj->fg, obj->bg, &w->font, w);
	
}
