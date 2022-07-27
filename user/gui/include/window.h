#ifndef __WINDOW_H
#define __WINDOW_H

#include "gui.h"

#include <data.h>


#define FONT_DATA_SIZE (128*16*2)


#define WMENU_SIZE 24
#define WBART_SIZE 24
#define WROD_SIZE 16

#define SCROLL_SIZE 8

#define WMENU_BAR_SIZE 36

#define TYPE_MENUBOX 1
#define TYPE_EDITBOX 2
#define TYPE_LISTBOX 3
#define TYPE_FILE_LISTBOX 4
#define TYPE_TERMINALBOX 5

#define MSG_MAX 10

extern int getkey();
extern unsigned long __window;

typedef struct _font {
	int x;
	int y;
	unsigned int fg_color;
	unsigned int bg_color;
	unsigned long buf;
}__attribute__ ((packed)) font_t;

typedef struct __WINDOW {
	unsigned int	spinlock;
	unsigned int	rx, ry, bpp, scanline;
	unsigned int 	width, height;
	unsigned int 	pos_x, pos_y;
	
	unsigned int 	area_width, area_height;
	unsigned int 	area_x, area_y;
	
	unsigned int  	fg, bg, text_fg;
	
	unsigned int 	cy, cx;
    unsigned int 	chcounter;
    unsigned int 	scrollx;
    unsigned int 	scrolly;
	font_t		    font;
    unsigned int 	t_y, t_x;
    unsigned int 	t_h, t_w;
	unsigned int    terminal;
    unsigned int    terminal_clear;
    unsigned long   terminal_buffer;
    unsigned long   stdin;
    unsigned long   stdout;
    unsigned long   stderr;

	unsigned int	style;
	unsigned int	gid;
    unsigned int    foco;
    unsigned long long component[256];
	char		    rsv[4096 - 176 - 2048];
	unsigned long 	start;
	
}__attribute__ ((packed)) WINDOW;

typedef struct {

    unsigned int 	x, y;
    unsigned int    size;
	unsigned int 	width;
    unsigned int 	flag;
    unsigned int 	index;
    unsigned long 	addr;

}__attribute__ ((packed)) XSCROLL;

typedef struct {
    unsigned int    magic;

    unsigned int    id;
	unsigned int	type;
    unsigned int	style;

    unsigned int	visible;
	
    unsigned int 	x, y;
	unsigned int 	width, height;
	
	unsigned int  	fg, bg;
	
	unsigned int 	cy, cx;

    unsigned int 	clear;

    unsigned int 	scroll;
    
    XSCROLL         scrollx;
    XSCROLL         scrolly;

    unsigned int	offset;
	unsigned int 	len;

	font_t		    font;
	
	unsigned int	handle;

    unsigned int	size;
	unsigned long 	addr;
}__attribute__ ((packed)) XCOMPONENT;


typedef struct _HANDLE {
	unsigned int 	type;
	unsigned int 	x, y;
	unsigned int 	width, height;
	 
	unsigned int	id, flag;
	unsigned int 	fg, bg;
	unsigned int 	cx, cy; 
	unsigned int	offset;
	unsigned int 	len;
	unsigned int 	bytes;
    unsigned int 	scroll;
	unsigned long	addr;
	unsigned long	w;
}__attribute__ ((packed)) HANDLE_T;


typedef struct _SUBMENU {
	unsigned int id;
	unsigned int style;
	char 	name[32];
}__attribute__ ((packed)) SUBMENU_T;
typedef struct _MENU {
	unsigned int 	type;
	unsigned int 	x, y;
	unsigned int 	width, height;
	
	unsigned int	id, flag;
	unsigned int 	fg, bg;
	unsigned int	count;
	unsigned long	w;
	char		name[32];
	SUBMENU_T 	sub[32];
}__attribute__ ((packed)) MENU_T; 


typedef struct _MSG {
	unsigned int 	offset1, offset2;
	unsigned int 	size;
	unsigned int	buf[MSG_MAX];
}__attribute__ ((packed)) MSG_T; 


void drawline(int x1, int y1, int x2, int y2, int rgb, WINDOW *w);
void drawrect(int x, int y, int width, int height, int rgb, WINDOW *w);
void drawchar( unsigned short int c, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w);
void drawstring( const char *str, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w);
void drawstring_trans( const char *str, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w);

void drawchar_trans( unsigned short int c, int cx, int cy, unsigned int fg, unsigned int bg, struct _font *font, WINDOW *w);

WINDOW *window(const char *title,int x, int y, int width, int height,int fg, int bg, int tfg , int style);
void wcl(WINDOW *w);

void __window_clear(WINDOW *w);

int BitMAP( void *Data, int X, int Y, int bg, WINDOW *w);
int BitMAP2( void *Data, int X, int Y, int fg, int bg, WINDOW *w);


void color(int rgb);


HANDLE_T *editbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id);
int m_edit(HANDLE_T *obj);
void update_editbox(HANDLE_T *obj);

void fmouse(WINDOW *w);

void menubox(WINDOW *w, MENU_T **menu, const char *name, int x1, int y1, int x2, int y2, unsigned int id);
int menumotor(MENU_T *menu);
int submenubox(MENU_T *menu, const char *name, unsigned int id);

int register_obj(void *obj);
int obj_process(int x, int y);
void update_objs(WINDOW *w);
int obj_focprocess();
int init_process(HANDLE_T *o);


int getmsg(int msg, WINDOW *w);

void msg_init();
void msg_write (int id);
int msg_read();

int dialogbox(WINDOW *w, void *buf);

void border(int top, int bottom, int left, int right, WINDOW *w);


HANDLE_T *file_listbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id);
int m_file_list(HANDLE_T *obj);

HANDLE_T *terminalbox(int x, int y, int width, int height, int fg, int bg, WINDOW *w, unsigned int id);
void trminalscroll(HANDLE_T *obj, int mouse_x, int mouse_y);
void m_terminalbox(HANDLE_T *obj);

#endif
