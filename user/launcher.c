#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <data.h>
#include <time.h>

#include <pipe.h>


#define APP_LIST_SIZE 1024


struct app
{
    unsigned long id;
    unsigned long status;
    unsigned int x1, y1, x2, y2; 
    unsigned long icone_addr; 
    unsigned long path_name_addr;
};


struct app app[APP_LIST_SIZE];
int app_index;
int app_index_foco;
unsigned long app_id;
int app_x, app_y;
int app_width, app_height;

char *app_pathname;
extern char *pwd;

extern int wp(char *name);
extern void update(const char *id, WINDOW *w );

// Fixar app na bara de tarefa
static int app_fixe(unsigned long status, const char *pathname, const char *icone_pathname, WINDOW *w)
{
    app[app_index].id = 0;
    app[app_index].status |= status;


    app[app_index].x1 = app_x + (app_index *(32 + 4));
    app[app_index].y1 = app_y;
    app[app_index].x2 = app_width;
    app[app_index].y2 = app_height;

    // 16 KiB
    unsigned long addr = (unsigned long) malloc(0x4000);
    app[app_index].path_name_addr = addr;
    app[app_index].icone_addr = addr + 0x1000;

    char *pa = (char*) app[app_index].path_name_addr; 
    strcpy(pa, pathname);

    char *data = (char*) app[app_index].icone_addr;

    // open icone
    FILE *f = fopen(icone_pathname,"r");
	if(f){
	
		fseek(f,0,SEEK_END);
		int size = ftell(f);
		rewind(f);
	

        if(size <= 0x3000) {
		    fread(data, 1, size, f);

		    BitMAP( data, app[app_index].x1 , app[app_index].y1, 0xe0e0e0, w);
	    }

		fclose(f);

	}else printf("open: %s error\n", icone_pathname );


    return app_index ++;
}


static int app_check_cursor()
{
    int x, y;

    if(mouse->b&0x1) 
	{
        while(mouse->b&0x1)__asm__ __volatile__("pause": : :"memory");

        x = mouse->x;
        y = mouse->y;

        for(int i=0; i < app_index; i ++) {
            int x1 = app[i].x1;
            int y1 = app[i].y1;
            int x2 = app[i].x2;
            int y2 = app[i].y2;

            if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2) ) 
            {
                return i;
            }
        }
    }

    return (-1);
}

static void app_remove_foco(WINDOW *w)
{
    if(app_index_foco == -1)
    {   
        return;
    }

    // remove foco
    app[app_index_foco].status &=~0x2;

    char *data = (char*) app[app_index_foco].icone_addr;

    BitMAP( data, app[app_index_foco].x1 , app[app_index_foco].y1, 0xe0e0e0, w);

    app_index_foco = -1;
}

static int app_execute(int index, WINDOW *w)
{
    char *data = (char*) app[index].icone_addr;

    if(app[index].status & 0x1)
    {
        if(app[index].status & 0x2) 
        {   // tem o foco 
            return 0;
        }else {            
            // foco
            app[index].status |= 0x2;
            app_remove_foco(w);
            app_index_foco = index;
            BitMAP( data, app[index].x1 , app[index].y1, 0, w);

            // foco
            __asm__ __volatile__("int $0x72"::"d"(8),"S"( app[index].id),"c"(5));

        }
    }
    else {
        app[index].id = app_id;
        app[index].status |= 0x1;

        app[index].status |= 0x2;
        app_remove_foco(w);
        app_index_foco = index;
        BitMAP( data, app[index].x1 , app[index].y1, 0, w);

        strcpy( app_pathname , (char*)app[index].path_name_addr );

        // execute
		__asm__ __volatile__( "movq %%rax,%%r8;"
		" int $0x72;"
		::"d"(8),"D"(app_pathname),"S"( app_id ),"c"(2), "a"(pwd));

        app_id ++;
    }

    return 0;
}


static void app_fixe_remove(WINDOW *w, unsigned long id)
{
    for(int i = 0; i < app_index; i ++)
    {
        if(app[i].id == id) {
            if(app[i].status & 0x2) app_remove_foco(w);
    
            app[i].id = 0;
            app[i].status = 0;

            break;
        }
    }
}

const char *ss[60]={\
"00","01","02","03","04","05","06","07","08","09",
"10","11","12","13","14","15","16","17","18","19",
"20","21","22","23","24","25","26","27","28","29",
"30","31","32","33","34","35","36","37","38","39",
"40","41","42","43","44","45","46","47","48","49",
"50","51","52","53","54","55","56","57","58","59"
};




int main()
{
    app_pathname = malloc(0x1000);
    app_index = 0;
    app_index_foco = -1;
    app_id = 1;
    memset(app, 0, sizeof(struct app) *APP_LIST_SIZE);


	time_t	timer;
	struct tm *h;
	char s[32];
	memset(s,0,32);
	
	int min = 0;
	
	h = gmtime(&timer);
	sprintf(s, "%s:%s",ss[h->tm_hour], ss[h->tm_min]);

	WINDOW *w = (WINDOW*) (__window);
	memset((char*)&w->start,0, 0x400000-0x1000);
	
	w->width = w->rx;
	w->height = w->ry;
	w->pos_x = 0;
	w->pos_y = 0;
	
	
	w->area_width = w->width;
	w->area_height = w->height;
	w->area_x = 0;
	w->area_y = 0;
	
	//init font
	w->font.x = 8;
	w->font.y = 16;
	w->font.fg_color = 0;//0xfcfc00;
	w->font.bg_color = 0xe0e0e0;
	w->font.buf = (unsigned long)font8x16;
	
    // area
	drawline(w->pos_x ,w->pos_y, w->width, w->height, 0x3030,w);
    //wp("w.ppm");

	// barra
	drawline(w->pos_x ,w->height-36, w->width, WMENU_BAR_SIZE, 0xe0e0e0,w);

	// register 
	wcl(w);

    // register pipe launcher
    __asm__ __volatile__("int $0x72"::"d"(11));

    // inicializa os icones fixados
    app_x = w->pos_x  +  4; 
    app_y = w->height - 34;
    app_width = 32;
    app_height = 28;

    app_fixe( 0, "explore.bin\0" , "folder.bmp\0" , w);
    app_fixe( 0, "term.bin\0" , "console.bmp\0" , w);
    app_fixe( 0, "editor.bin\0" , "edit.bmp\0" , w);
	
	for(;;){

        int index = app_check_cursor();
        if(index != -1) {
            app_execute(index, w);
        }
	
	
		if(min != h->tm_min) {
			min = h->tm_min;
			sprintf(s, "%s:%s",ss[h->tm_hour], ss[h->tm_min]);
			update(s, w );
		}

        __pipe_t pipe;

        if( pipe_read_2x ( &pipe, __pipe__) )
        {
            //printf("PIPE: %x %x %x %x %lx\n", pipe.id, pipe.r1, pipe.r2, pipe.r3, pipe.r4);
            app_fixe_remove(w, pipe.r4);

        }
		
	}

	return 0;
}
