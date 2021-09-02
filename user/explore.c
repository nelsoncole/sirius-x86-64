#include <window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <file.h>

#define XX 540
#define YY 560

#define ID_FILE 	(0x01 + 0)

int x, y;

#define ID_MENU_FILE 	(0x10 + 0)
#define ID_MENU_EXIT 	(ID_MENU_FILE + 4)
#define ID_MENU_HELP 	(0x20 + 0)

MENU_T *menu_file, *menu_help;

static void loop(WINDOW *w);
int main()
{
	x = 150;
	y = 20;
	
	directory_entry_t *d = (directory_entry_t *) malloc(0x2000);
	


	WINDOW *w = window("Files",16, 0, XX, YY, 0x1f1f2f, 0xc0c0c0, 0, 0x10);

	HANDLE_T *file = file_listbox( x , y, w->area_width - x,  w->area_height - y , 0x000000, 0xFFFFFF, w, ID_FILE);
	register_obj(file); // registrar objecto


    menubox(w, &menu_file, "File\0",0,0, 8*5, 24, ID_MENU_FILE);
	submenubox(menu_file, "Exit\0",  ID_MENU_EXIT);
	menubox(w, &menu_help, "Help\0",8*5+8,0, 8*5, 24, ID_MENU_HELP);

    register_obj(menu_file); // registrar objecto
	register_obj(menu_help); // registrar objecto
	
	
	// register 
	wcl(w);
	init_process(file);
	
	__asm__ __volatile__("int $0x72"::"d"(3),"D"(stdin)); // foco do teclado

	
	for(;;) loop(w);
	
	return 0;
}

static void loop(WINDOW *w)
{
	int msg = getmsg(0, w);
    switch( msg ){
        case ID_FILE:
			//printf("SIM\n");
		break;

		case ID_MENU_EXIT:
             exit(0);
		break;
		case ID_MENU_HELP:

		break;
			
	
	}	
}
