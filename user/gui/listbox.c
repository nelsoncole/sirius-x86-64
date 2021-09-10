#include <window.h>
#include <stdlib.h>
#include <string.h>
#include <file.h>
#include <time.h>

#define ESP_SIZE 16

static void editbox_refresh(HANDLE_T *obj);
static void attr(directory_entry_t *d);
static char file_data[32];
static char file_type[32];
static char *file_unidade[]={"KIB", "MiB", "GiB"};

HANDLE_T *file_listbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id) {
	if(w == 0) return 0;
	HANDLE_T *obj = malloc(sizeof(HANDLE_T));
	if(obj == 0) return 0;

	memset(obj, 0, sizeof(HANDLE_T) );
	
	obj->w  = (unsigned long) w;
	obj->id = id;
	obj->x  = w->area_x + x;
	obj->y  = w->area_y + y;
	
	obj->type = TYPE_FILE_LISTBOX;
	
	if(width >= w->area_width) obj->width  = w->area_width;
	else obj->width  = width;
	if(height >= w->area_height) obj->height  = w->area_height;
	else obj->height = height;
	
	obj->fg = fg;
	obj->bg = bg;
	obj->len = 0x2000; // 8KiB
	obj->addr = (unsigned long) malloc(obj->len);
	if(obj->addr != 0) memset((char*)obj->addr, 0, obj->len );
	
	//area
	drawline( obj->x ,obj->y, obj->width, obj->height , obj->bg, w);
	
	drawrect(obj->x, obj->y, obj->width, obj->height, 0x808080, w );
	
	return obj;
}

int m_file_list(HANDLE_T *obj) {
	int c;
	if(!obj) return -1;
	
	c = getkey();
	if(c == 0x1b) {
		c = getkey();
		if(c == '[') {
			c = getkey();
			
			switch(c) {
				case 'A':
					if(obj->offset > 0) obj->offset --;
				break;
				case 'B':
					//if(obj->offset < obj->bytes) obj->offset ++;
					obj->offset ++;
				break;
			}
			
			editbox_refresh(obj);
		
		}
		
		return 0;
	}
	
	// wait 1 segundo
	// Apenas quando houver evento de disco
	time_t	timer;
	struct tm *h = gmtime(&timer);
	if(obj->cy < h->tm_sec) {
		obj->cy = h->tm_sec;
		if(obj->cy >= 59) obj->cy = 0;
		editbox_refresh(obj);
	}
	
	return 0;
}

static void editbox_refresh(HANDLE_T *obj)
{
	WINDOW *w = (WINDOW *) obj->w;
	directory_entry_t *d = (directory_entry_t *) obj->addr;
	int count = open_dir("./", d, 64);
	
	int x = 0;
	int y =  obj->height/ESP_SIZE ;

	while(w->spinlock) __asm__ __volatile__("pause;");
	w->spinlock ++;
				
	for(int i=0; i < y; i ++) {
	
		int fg = obj->fg , bg = obj->bg;
	
		if(i == obj->offset ) {
			bg = 0xD2691E;
			fg = -1;
		}
		
		drawline(obj->x + 4, obj->y + (ESP_SIZE*i) + 4 , obj->width - 8, ESP_SIZE, bg, w);
		
		if(count > i) {
			attr(d);
			drawstring_trans(d->file_name, obj->x + 4 + x, obj->y + (ESP_SIZE*i) + 4, fg, bg, &w->font, w);
			x += obj->width/2;
			drawstring_trans(file_data, obj->x + 4 + x, obj->y + (ESP_SIZE*i) + 4, fg, bg, &w->font, w);
			x += obj->width/4;
 			drawstring_trans(file_type, obj->x + 4 + x, obj->y + (ESP_SIZE*i) + 4, fg, bg, &w->font, w);
			x = 0;
			d ++;
		}
	
	}
	
	w->spinlock = 0;
	
}


static void attr(directory_entry_t *d) 
{
	if(d->attr&0x20) {
 		sprintf(file_type, "%s", "File");
 		double a = d->file_size;
 		double b = 1024;
 		int u = 0;
 		if(d->file_size >= 0x40000000) { 
 			u = 2;
 			b = 0x40000000;
 			
 		}else if(d->file_size >= 0x100000) {
 		 	u = 1;
 		 	b = 0x100000;
 		}
 				
 				
 		double size = a/b;
 		sprintf(file_data, "%lf", size);
 		int of = strlen(file_data);
 		for(int i = of; i < 7; i++) file_data[i] = ' ';
 		sprintf(file_data + 7, "%s", file_unidade[u]);
 			
 		} else {
 			sprintf(file_type, "%s", " \0");	
 			sprintf(file_data, "%s", " \0");
 		}
} 
