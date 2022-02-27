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
char *app_memory;
static void loop(WINDOW *w);

void main() {
    WINDOW *z = (WINDOW*) __window;
    double aspect_ratio = 16.0 / 9.0;
    int image_width = z->rx - (z->rx/4);
    int image_height = (const int)(image_width / aspect_ratio);
	
    app_memory = (char*)malloc(0x80000); // 512
	WINDOW *w = window("@~Terminal",0, 0, image_width, image_height, 0x1f1f2f, 0x80101020, 0xFFFFFF, 0x30);

    w->style |= 0x80000000;
    w->stdin = (unsigned long)stdin;
    w->terminal = 0x1234; 
	
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

    FILE *fp = fopen(path,"r+b");
    if(fp) {
        fseek(fp,0,SEEK_END);
	    int size = ftell(fp);
	    rewind(fp);
        fread (app_memory, 1, size, fp);
	    fclose(fp);

        __asm__ __volatile__("movq %%rax,%%r8;"
	    " int $0x72;"
	    ::"d"(8),"D"(app_memory),"S"(0),"c"(1), "a"(pwd)); // fork

    }else {
        printf("fopen error: \'%s\'\n", path);
	}

	while(1) loop(w);
}

static  void loop(WINDOW *w)
{
	int msg = getmsg(0, w);
	switch( msg ){
		case ID_MENU_EXIT:
            free(app_memory);
            exit(0);
		break;
		case ID_MENU_HELP:

		break;
        case ID_TERMINAL:
            
        break;
			
	}	
}
