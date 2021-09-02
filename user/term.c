#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <pipe.h>


#define ID_MENU_FILE 	(0x10 + 0)
#define ID_MENU_EXIT 	(ID_MENU_FILE + 4)

#define ID_MENU_HELP 	(0x20 + 0)

extern char *pwd;
MENU_T *menu_file, *menu_help;

static void loop(WINDOW *w);

void put_pixel_vetor(int x, int y, unsigned int color, int v, WINDOW *w) {


        unsigned char r = color >> 16 &0xff;
        unsigned char g = color >> 8 &0xff;
        unsigned char b = color &0xff;

        unsigned char rgb[4]={0, 0, 0, 0};

        rgb[0] = (b*93)/100;
        rgb[1] = (g*93)/100;
        rgb[2] = (r*93)/100;

   
        put_pixel( w->area_x + x,  w->area_y + y, w->width, *(unsigned int*)rgb, &w->start);
        
        if(!v) x ++;
        else y ++;

        rgb[0] = (b*16)/100;
        rgb[1] = (g*35)/100;
        rgb[2] = (r*35)/100;

        put_pixel( w->area_x + x,  w->area_y + y, w->width, *(unsigned int*)rgb, &w->start);

        if(!v) x ++;
        else y ++;

        rgb[0] = (b*0)/100;
        rgb[1] = (g*90)/100;
        rgb[2] = (r*90)/100;

        put_pixel( w->area_x + x,  w->area_y + y, w->width, *(unsigned int*)rgb, &w->start);

}

void main() {
	
	WINDOW *w = window("@~Terminal",0, 0, 500, 400, 0x1f1f2f, 0x80101020, 0xFFFFFF, 0x10);

    w->style |= 0x80000000; 
	
	menubox(w, &menu_file, "File\0",0,0, 8*5, 24, ID_MENU_FILE);
	submenubox(menu_file, "Exit\0",  ID_MENU_EXIT);
	menubox(w, &menu_help, "Help\0",8*5+8,0, 8*5, 24, ID_MENU_HELP);

    register_obj(menu_file); // registrar objecto
	register_obj(menu_help); // registrar objecto

	// register 
	wcl(w);
/*
    int x = 80;

    for(int y= 80; y > 0; y --) {

        if(y%2) {
            x ++;
            put_pixel_vetor( x , 80 +y, 0xc0c0c0, 1, w);
        }
        else
            put_pixel_vetor( x , 80 +y, 0xc0c0c0, 1, w);
    }


    for(;;);*/
	
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
			
	
	}	
}
