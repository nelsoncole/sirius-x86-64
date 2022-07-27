#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <pipe.h>

#include <sys/communication.h>


#define ID_MENU_FILE 	(0x10 + 0)
#define ID_MENU_EXIT 	(ID_MENU_FILE + 4)

#define ID_MENU_HELP 	(0x20 + 0)

#define ID_TERMINAL 	(0x30 + 0)

extern char *pwd;
MENU_T *menu_file, *menu_help;
char *app_memory;
static void loop(WINDOW *w);

unsigned long Xcomponent(unsigned int id, unsigned int type, unsigned int style, unsigned int x, unsigned int y,
unsigned int width, unsigned int height, unsigned int fg, unsigned int bg, unsigned int size){
    XCOMPONENT *c = malloc(sizeof(XCOMPONENT));
    memset(c,0,sizeof(XCOMPONENT));
    
    c->magic = 1234;

    c->id = id;

    c->type = type;
    c->style = style;
	
    c->x = x;
    c->y = y;
	c->width=width;
    c->height=height;
	
	c->fg =fg; c->bg=bg;
	
	c->cy=0; c->cx=0;

    c->clear=0;

    c->scroll=0;

    c->offset=0;
	c->len=0;
	
	c->handle=0;
    c->size = size;
	c->addr = (unsigned long)malloc(size);
    memset((char*)c->addr,0,size);

    //init font
	c->font.x = 8;
	c->font.y = 16;
	c->font.fg_color = fg;
	c->font.bg_color = bg;

    char *tmp = (char*) malloc(FONT_DATA_SIZE);
	memcpy(tmp, font8x16, FONT_DATA_SIZE);

    c->font.buf = (unsigned long)tmp;

    return (unsigned long)c;
}

void Xcomponent_display(WINDOW *w){
    XCOMPONENT *c = 0;
    unsigned int x, y;
    for(int i=0; i < 256; i++)
    {
        c =(XCOMPONENT *)w->component[i];
        if(!c) break;
        if(c->width == -1) c->width = w->area_width;
        if(c->height == -1) c->height = w->area_height;

        c->x += w->area_x;
        c->y += w->area_y;
        x = c->x;
        y = c->y;
        
        
        switch(c->type)
        {
            case 1:
                drawline(x , y, c->width, c->height, c->bg, w);
                // scroll
                c->width -= 16;
                drawline(x + c->width + 4, y+2, 8, c->height-4, 0xe0e0e0, w);
                c->scrolly.width = c->height-4;
            break;
        }

    }
}

void main() {
    WINDOW *z = (WINDOW*) __window;
    double aspect_ratio = 16.0 / 9.0;
    int image_width = z->rx - (z->rx/4);
    int image_height = (const int)(image_width / aspect_ratio);
	
    image_width = 600; image_height = 500;

    app_memory = (char*)malloc(0x80000); // 512
	WINDOW *w = window("@~Terminal",0, 0, image_width, image_height, 0x1f1f2f, 0x80101020, 0xFFFFFF, 0x10);

    w->style |= 0x80000000;

    w->terminal = 0x1234; 
    

    unsigned long x = Xcomponent(0x11, 1, 0, 0, 0, -1, -1, 0xffffff, 0x80101020, 0x10000);
    w->component[0] = x;
    Xcomponent_display(w);

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

        /*__asm__ __volatile__("movq %%rax,%%r8;"
	    " int $0x72;"
	    ::"d"(8),"D"(app_memory),"S"(0),"c"(1), "a"(pwd)); // fork*/
        unsigned long pid = 0;
        // getpid()
        __asm__ __volatile__("int $0x72":"=a"(pid):"d"(1),"c"(0));
        struct communication commun, commun2;
        memset((char*)&commun, 0, sizeof(struct communication));
        commun.type = COMMUN_TYPE_EXEC_CHILD;
        commun.pid = pid;
        unsigned long *addr = (unsigned long*)((unsigned long)&commun.message);
        addr[0] = (unsigned long)((unsigned long*)app_memory);
        strcpy( (char*)&addr[1],pwd);
        char *arg = (char*)&addr[1];
        arg += strlen(pwd) + 1;
        strcpy( arg, path);
        communication(&commun, &commun2);

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
