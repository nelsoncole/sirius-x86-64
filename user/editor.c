#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <pipe.h>

#define ID_EDIT 	(0x01 + 0)

#define ID_MENU_FILE 	(0x10 + 0)
#define ID_MENU_OPEN	(ID_MENU_FILE + 1)
#define ID_MENU_SAVE 	(ID_MENU_FILE + 2)
#define ID_MENU_CLOSE 	(ID_MENU_FILE + 3)
#define ID_MENU_EXIT 	(ID_MENU_FILE + 4)

#define ID_MENU_HELP 	(0x20 + 0)

#define X 500
#define Y 400

char filename[128];
HANDLE_T *edit;
int save;

void loop(WINDOW *w);



static int gravar(HANDLE_T *obj) 
{
	FILE *f = fopen(filename, "w");
	if(!f) return -1;

	int len = obj->bytes;
	char *buf = (char*) obj->addr;

	fwrite(buf,1,len,f);

	fflush(f);
	fclose(f);
	return 0;
}

void main() {
	
	save = 0;
	
	WINDOW *w = window("Editor",1, 40, X, Y, 0x067AB5 /*0x1f1f2f*/, 0xFFFFFF, 0, 0x10);
	
	edit = editbox(0, 0, X, Y, 0, 0xEFEFEF, w, ID_EDIT );
	register_obj(edit); // registrar objecto
	
	//buttun(w);
	
	MENU_T *menu, *menu1;
	
	menubox(w, &menu, "File\0",0,0, 8*5, 24, ID_MENU_FILE);
	submenubox(menu, "Open\0",  ID_MENU_OPEN);
	submenubox(menu, "Save\0",  ID_MENU_SAVE);
	submenubox(menu, "Close\0", ID_MENU_CLOSE);
	submenubox(menu, "Exit\0",  ID_MENU_EXIT);
	
	menubox(w, &menu1, "Help\0",8*5+8,0, 8*5, 24, ID_MENU_HELP);
	
	
	register_obj(menu); // registrar objecto
	register_obj(menu1); // registrar objecto
	
	
	// register 
	wcl(w);
	
	init_process(edit);
	
	__asm__ __volatile__("int $0x72"::"d"(3),"D"(stdin)); // foco do teclado e mouse
	
		
	while(1) loop(w);
}


void loop(WINDOW *w)
{
	int msg = getmsg(0, w);
	switch( msg ){
		case ID_MENU_OPEN:
		
		break;
		case ID_MENU_SAVE:

			if(save == 0) {
				dialogbox(w,filename);
				save = 1;
				gravar(edit);
			} else {
			
				gravar(edit);
			}
			
		break;
		case ID_MENU_CLOSE:
	
		break;
		case ID_MENU_EXIT:
            exit(0);
		break;
		case ID_MENU_HELP:

		break;
			
	
	}	
}
