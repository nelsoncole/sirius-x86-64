#include <window.h>
#include <data.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>


void menubox(WINDOW *w, MENU_T **menu, const char *name, int x1, int y1, int x2, int y2, unsigned int id )
{

    if(!w) return;
    if( !(w->style & 0x10) ) return;

	MENU_T *m = (MENU_T*) malloc(sizeof(MENU_T));
	memset(m, 0, sizeof(MENU_T));
	
	int len = strlen(name);
	if(len > 31) len = 31;
	memcpy(m->name, name, len);
	
	m->type = TYPE_MENUBOX;
	
	m->id = id;
	
	m->x = x1;
	m->y = y1 +  WBART_SIZE;
	
	m->width = x2;
	m->height = y2;
	
	m->w = (unsigned long) w;
	
	drawstring( m->name, m->x+8, m->y+4, 0, 0xe0e0e0, &w->font, w);
	
	
	*(MENU_T**)(menu) = m;
}

int submenubox(MENU_T *menu, const char *name, unsigned int id)
{
    if(!menu) return -1;
	WINDOW *w = (WINDOW*) menu->w;
    if( !(w->style & 0x10) ) return -1;
	
	char *dst = (char*) menu->sub[menu->count].name;
	menu->sub[menu->count++].id = id;
	
	
	int len = strlen(name);
	if(len > 31) len = 31;
	memcpy(dst, name, len);
	
	return 0;
}


int menumotor(MENU_T *menu)
{
	int ret = 0;
	int idx = -1;
	WINDOW *w = (WINDOW*) menu->w;
			
	drawstring( menu->name, menu->x+8, menu->y+4, 0xff0000, 0xe0e0e0, &w->font, w);
	drawstring_trans( "____", menu->x+8, menu->y+4, 0xff0000, 0xe0e0e0, &w->font, w);
	
	int x1 = w->area_x + menu->x;
	int y1 = w->area_y;
	int x2 = 100;
	int y2 = 20;
	int bg = 0x808080;
	
	unsigned int mb = 0; 
	
	while(mouse2->b&0x1); // espera soltar
	
	if(menu->count > 0){
	
		drawline(x1, y1, x2, y2*menu->count+4, bg, w);
		
		do{
			//while(w->spinlock);
			//w->spinlock++;
			
			for(int i=0; i < menu->count; i ++ ){
				
				int x = mouse2->x - w->pos_x;
				int y = mouse2->y - w->pos_y;
				
				mb = mouse2->b;
				
				char *name = (char*) menu->sub[i].name;
				
				if( x > x1 && y > (y1 + y2*i + 4) && x < (x1+x2) && y < ((y1 + y2*i)+y2) ) {
					//drawstring("        ", x1+8, (y2*i) + y1+4, 0, bg, &w->font, w);
					drawstring( name, x1+16, (y2*i) + y1+4, -1, bg, &w->font, w);
					//drawstring_trans("_____", x1+8, (y2*i) + y1+4, 0, bg, &w->font, w);
					
					idx = i;
					
					i --;
					
					if(mb&0x1) {
						break;
					}
					
				}else {
				
					//drawstring("        ", x1+8, (y2*i) + y1+4, 0, bg, &w->font, w);
					drawstring( name, x1+16, (y2*i) + y1+4, 0, bg, &w->font, w);
					
					idx = -1;
				}
				
			}
			
			//w->spinlock = 0;
			
		}while((mb&0x1) == 0); // espera clicar
		
		if(idx != -1) msg_write (menu->sub[idx].id);
		
	
		ret = 1;
	}
	
	while(mouse2->b&0x1); // espera soltar
		
	drawstring( menu->name, menu->x+8, menu->y+4, 0, 0xe0e0e0, &w->font, w);
			
	//printf("MENU: %s\n", menu->name);
	
	return ret;

}
