#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <pipe.h>


#define ID_MENU_FILE 	(0x10 + 0)
#define ID_MENU_EXIT 	(ID_MENU_FILE + 4)

#define ID_MENU_HELP 	(0x20 + 0)

#define ID_TERMINAL 	(0x30 + 0)

extern char *pwd;
MENU_T *menu_file, *menu_help;

static void loop(WINDOW *w);

void main() {
	
	WINDOW *w = window("@~Terminal",100, 20, 600, 500, 0x1f1f2f, 0x80101020, 0xFFFFFF, 0x10);

    w->style |= 0x80000000; 
	
	menubox(w, &menu_file, "File\0",0,0, 8*5, 24, ID_MENU_FILE);
	submenubox(menu_file, "Exit\0",  ID_MENU_EXIT);
	menubox(w, &menu_help, "Help\0",8*5+8,0, 8*5, 24, ID_MENU_HELP);

    register_obj(menu_file); // registrar objecto
	register_obj(menu_help); // registrar objecto



    HANDLE_T *obj = terminalbox(0, 0, -1, -1, 0x00FF00, 0x80101020, w, ID_TERMINAL);
    register_obj(obj); // registrar objecto

	// register 
	wcl(w);

	__asm__ __volatile__("int $0x72"::"d"(3),"D"(stdin)); // foco do teclado

	char *path = malloc(128);

	strcpy(path,"shell.bin\0");
	__asm__ __volatile__("movq %%rax,%%r8;"
	" int $0x72;"
	::"d"(8),"D"(path),"S"(0),"c"(1), "a"(pwd)); // fork
	
	while(1) loop(w);
}

static  void loop(WINDOW *w)
{
	int msg = getmsg(0, w);
	switch( msg ){
		case ID_MENU_EXIT:
             exit(0);
		break;
		case ID_MENU_HELP:

		break;
        case ID_TERMINAL:
            
        break;
			
	}	
}
