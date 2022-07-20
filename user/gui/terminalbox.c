#include <window.h>
#include <stdlib.h>
#include <string.h>


void trminalscroll(HANDLE_T *obj, int mouse_x, int mouse_y){
    if(!obj) return;
    WINDOW *w = (WINDOW*) obj->w;

    if( (mouse_x - w->area_x) < (obj->width - 0) ) return;

    while(w->spinlock != 0) __asm__ __volatile__("pause;");
    w->spinlock ++;

    drawrect((obj->x + obj->width - 1) - 0, obj->y + 1, 1, obj->height - 2, 0x808080, w);
    // clean
    drawline( (obj->x + obj->width - 0) - 1 ,obj->y + 2 + obj->scroll, 6, 40 , w->bg, w);
    // update
    obj->scroll = mouse_y - w->area_y;

    drawline( (obj->x + obj->width - 0) - 1 ,obj->y + 2 + obj->scroll, 6, 40 , 0xe0e0e0, w);

    w->spinlock = 0;
}

HANDLE_T *terminalbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id) {
	if(w == 0) return 0;
	HANDLE_T *obj = malloc(sizeof(HANDLE_T));
	if(obj == 0) return 0;

	memset(obj, 0, sizeof(HANDLE_T) );
	
	obj->w  = (unsigned long) w;
	obj->id = id;
	obj->x  = w->area_x + x;
	obj->y  = w->area_y + y;
	
	obj->type = TYPE_TERMINALBOX;
	
	if(width >= w->area_width) obj->width  = w->area_width - x;
	else obj->width  = width;
	if(height >= w->area_height) obj->height  = w->area_height - y;
	else obj->height = height;
	
	obj->fg = fg;
	obj->bg = bg;
	obj->len = 0x10000; // 64KiB
	obj->addr = (unsigned long) malloc(obj->len);
	if(obj->addr != 0) memset((char*)obj->addr, 0, obj->len );
	
	//area
	drawline( obj->x ,obj->y, obj->width, obj->height , obj->bg, w);
	drawrect(obj->x, obj->y, obj->width, obj->height, 0x808080, w);

    // scroll
    obj->scroll = 0;
    drawline( (obj->x + obj->width) + 1, obj->y+1, 6, obj->height-2 , 0xe0e0e0, w);
    //drawrect((obj->x + obj->width - 2) - 0, obj->y + 1, 1, obj->height - 2, 0x808080, w);

	return obj;
}


void m_terminalbox(HANDLE_T *obj){

}
