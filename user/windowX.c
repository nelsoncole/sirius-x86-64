#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/communication.h>
#include <sys/exectve.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <window.h>

#include <data.h>

#define true 1
#define false 0

typedef struct _CLIENT
{
	unsigned long 	w;
	unsigned long	spinlock;
    unsigned long   pid;
    unsigned long   id;
    unsigned long   flag;
    unsigned short  udp_port;
    struct _CLIENT  *tail;
	struct _CLIENT  *next;
}__attribute__ ((packed)) CLIENT;


typedef struct {
  	char *name;
    char *path;
  	int type;
    int idex;
    int count;
    int id;
}__attribute__((packed)) MENULIST;

MENULIST menu_ambiente[] = {
    {"Criar Nova Pasta", "", 0, 0, 5,0x10},
    {"Abrir Terminal", "term.bin", 0,0,0,0x11},
    {"Abrir ChatIRC", "chatirc.bin", 0,0,0,0x12},
    {"", "", 2, 0, 0, 0},
    {"Sair", "", 1, 0, 0, 0},
};

MENULIST submenu_ambiente[] = {
    {"Reiniciar", "", 0, 0, 3, 2},
    {"Desligar", "", 0, 0, 0, 3},
    {"Suspender", "", 0, 0, 0, 4},
};

MENULIST *global_menu;
MENULIST *global_submenu;
static CLIENT *client, *client_ready_queue;
static unsigned long   client_id;
extern char *pwd;
static int foco_status;
static int spinlock;
static void server();
static void *f1();
static struct _font *font;
static int recorde_xy;
static int record_xy2;
static int record_type;
static int process;
static WINDOW *w1;
static WINDOW *w2;
static WINDOW *w3;
static WINDOW *w4;
static int menu_message_id;
static int menu_ambiente_flag;
int fill_menu(MENULIST *menu, int count, int x, int y, WINDOW *w);
static void client_listing(unsigned long w, unsigned long pid, unsigned short udp_port);
struct con *con;

int main(int argc, char **argv)
{	
    con =(struct con *)malloc(sizeof(struct con));
    con_init(con);

    spinlock = 0;
    foco_status = false;
    pthread_t pthread;
    int i1;
    process = 0;
    menu_message_id = 0;
    menu_ambiente_flag = 0;
    global_menu = menu_ambiente;
    global_submenu = submenu_ambiente;
    client = client_ready_queue = (CLIENT*)malloc(sizeof(CLIENT));
    memset(client, 0, sizeof(CLIENT));
    client_id = 1;
    //*****************
    w4 = 0;
    w3 = 0;
    w1 = (WINDOW*)  (__window);
    w2 = (WINDOW*) create_new_window(w1, 400, 400);

    font = (struct _font *)malloc(sizeof(struct _font));
    char *tmp = (char*) malloc(FONT_DATA_SIZE);
	memcpy(tmp, font8x16, FONT_DATA_SIZE);
    font->buf = (unsigned long)tmp; 
    font->x = 8;
	font->y = 16;
	font->fg_color = 0;
	font->bg_color = 0xFFFFFF;

    w1 = init_window(0, 0, 80, 160, 0, 0xff0000, 0, w1);
    wcl(w1);
    w1->visibility = 0;
    w2 = init_window(0, 0, 80, 160, 0, 0xff0000, 0, w2);
    wcl(w2);
    w2->visibility = 0;

    
    pthread_create(&pthread,NULL,f1,&i1);
    pthread_join(pthread,NULL);

	server();
	return 0;
}

void client_listing(unsigned long w, unsigned long pid, unsigned short udp_port){
    CLIENT *new = (CLIENT*)malloc(sizeof(CLIENT));
    memset(new, 0, sizeof(CLIENT));

    new->w = (unsigned long) w;
    new->pid = pid;
    unsigned long id = 0;
    __asm__ __volatile__("int $0x72":"=a"(id):"d"(29),"D"(new->w));
    new->id = id;
    new->flag = 1;
    new->udp_port = udp_port;
    // adicionar no final da lista
    new->next = client;

    while(spinlock)spinlock++;

    client = new;
    // foco 
    spinlock = 0;
}

void xcomponent(int x, int y, WINDOW *w){
    if(!w)return;

    x -= w->pos_x;
    y -= w->pos_y;

    // foco
    if(w3){
        if(w3->foco == true){

        }
    }

    if(w3){
        int x1 = w3->pos_x;
        int y1 = w3->pos_y;
        int x2 = w3->width;
        int y2 = w3->height;
        if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2)) 
        {
            return;
        }else{
            // call default
            switch(w3->type){
                case WINDOW_TYPE_BUTTON:
                    handler_button(w, w3, 0);
                break;
                case WINDOW_TYPE_TEXTBOX:
                    if(w4){
                        if(w4->foco == false)
                            handler_textbox(w, w3, 0);   
                    }
                    else{
                        handler_textbox(w, w3, 0);
                    }
                break;
            }
            w3 = 0;
        }
    }

    WINDOW *tmp = (WINDOW *) w;
    while(tmp){
        WINDOW *window = tmp;
        int x1 = window->pos_x;
        int y1 = window->pos_y;
        int x2 = window->width;
        int y2 = window->height;
        if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2)) 
        {
            // processar...
            if(w3 != window){
                switch(window->type){
                    case WINDOW_TYPE_BUTTON:
                        handler_button(w, window, 1);
                        w3 = window;
                    break;
                    case WINDOW_TYPE_TEXTBOX:
                        handler_textbox(w, window, 1);
                        w3 = window;
                    break;
                }
            }
        }

        tmp = tmp->next;
    }
}

void event_xcomponent(WINDOW *w){
    if(!w3)return;

    switch(w3->type){
        case WINDOW_TYPE_BUTTON:
            handler_button(w, w3, 2);
        break;
        case WINDOW_TYPE_TEXTBOX:
            //handler_textbox(w, w3, 0);
            w3->foco = true;
            w4 = w3;
        break;
    }
}

void default_xcomponent(WINDOW *w, WINDOW *cw){
    if(!cw)return;

    switch(cw->type){
        case WINDOW_TYPE_BUTTON:
            handler_button(w, cw, 0);
        break;
        case WINDOW_TYPE_TEXTBOX:
            handler_textbox(w, cw, 0);
        break;
    }
}

void acknowledge(){
    if(!w3)return;
    if(w3->id < 32){ // systema
        CLIENT *tmp = client;
        switch(w3->id){
            case 1:
                // exit
                //
                client = client->next;
                foco_status = false;
                WINDOW *w = (WINDOW*)tmp->w;
                w->visibility = false;
                //communication(&commun, &commun2, 1024);
                con->req.type = COMMUN_TYPE_EXIT;
                con->req.pid = tmp->pid;
                con->serb.sin_addr.s_addr    = inet_addr("127.0.0.1");
                con->serb.sin_port = htons(1024);
                con_sendto(con);

                free(tmp);

                // foco
                if(client->w != 0){
                    con->req.type = COMMUN_TYPE_FOCO;
                    con->req.pid = 0;
                    con->req.apid = client->id;
                    con_sendto(con);
                }
            break;
        }
    }else{
        //sendto
        con->req.type = w3->id;
        con->serb.sin_addr.s_addr    = inet_addr("127.0.0.1");
        con->serb.sin_port = htons(client->udp_port);
        con_sendto(con);
    }
}

// Leitura do mouse
// Abrir menu de ambiente
// Abrir menu bar
// 
static void *f1(){
    char *argv[4];
    argv[0] = (char*)malloc(0x1000);
    char sinal = 0;
    struct communication commun, commun2;
    while(1){ // 

        // mouse *****
        int x = mouse->x;
        int y = mouse->y;

        if(mouse->b&0x2){ // RIGHT BTN
            if(w4){
                w4->foco == false;
                default_xcomponent((WINDOW *) client->w, w4);
            }

            if(!sinal){
                init_window(x, y, 160, 24*global_menu[0].count+16, 0, 0xC0C0C0, 0, w1);
                sinal = 1;
                record_xy2 = recorde_xy = 8000;
                record_type = -1;
                fill_menu(global_menu, global_menu[0].count,x, y, w1);
                w1->visibility = 1;
                menu_ambiente_flag = true;
                while(mouse->b&0x2);
            }else{
                while(mouse->b&0x2);
                w1->visibility = 0;
                w2->visibility = 0;
                process = 0;
                sinal = 0;
                menu_ambiente_flag = false;
            }
        }

        if(mouse->b&0x1){ // LEFT BTN
            if(w4){
                w4->foco == false;
                default_xcomponent((WINDOW *) client->w, w4);
            }
            if(client != 0){
                // ja tem o foco.
                // onclik
                event_xcomponent((WINDOW *) client->w);
            }

            while(mouse->b&0x1); // wait

            if(menu_ambiente_flag == true){
                w1->visibility = 0;
                w2->visibility = 0;
                process = 0;
                sinal = 0;
                // Processa o evento de menu
                // Improviso
                if(menu_ambiente_flag == true)
                switch(menu_message_id){
                    case 2:
                        commun.type = COMMUN_TYPE_REBOOT;
                        communication(&commun, &commun2, 1024);
                    break;
                    case 3:
                        commun.type = COMMUN_TYPE_POWEROFF;
                        communication(&commun, &commun2, 1024);
                    break;
                    case 0x11:
                        // execute 
                        strcpy(argv[0],"term.bin");
                        exectve(1, argv, 0, client_id++);
                    break;
                    case 0x12:
                        // execute 
                        strcpy(argv[0],"chatirc.bin");
                        exectve(1, argv, 0, client_id++);
                    break;
                }
                menu_ambiente_flag = false;
            }else{
                CLIENT *tmp = client;
                while(spinlock)spinlock++;
                while(tmp){
                    if(tmp->flag == 0){
                        tmp = tmp->next;
                        continue;
                    }
                    WINDOW *window = (WINDOW *) tmp->w;
                    int x1 = window->pos_x;
                    int y1 = window->pos_y;
                    int x2 = window->width;
                    int y2 = window->height;
                    if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2) ) 
                    {
                        // foco...
                        if(client == tmp){
                            // ja tem o foco.
                            // dispara o evento
                            acknowledge();
                            w3 = 0;
                        }
                        else{
                            w3 = 0;
                            /*commun.type = COMMUN_TYPE_FOCO;
                            commun.pid = 0;
                            commun.apid = tmp->id;
                            communication(&commun, &commun2, 1024);*/
                            con->req.type = COMMUN_TYPE_FOCO;
                            con->req.pid = 0;
                            con->req.apid = tmp->id;
                            con->serb.sin_port = htons(1024);
                            con_sendto(con);
                            CLIENT *tmp2 = client;
                            while(tmp2){
                                if(tmp2->next == tmp){
                                    tmp2->next = tmp->next;
                                }
                                tmp2 = tmp2->next;
                            }
                            tmp->next = client;
                            client = tmp;
                        }
                        break;
                    }
                    tmp = tmp->next;
                }
                spinlock = 0;
            }
        }

        if(sinal == 1){
            // configure evento do mouse
            fill_menu(global_menu, global_menu[0].count, x, y, w1);
        }

        //*** end mouse
        xcomponent(x, y, (WINDOW *) client->w);

    }

    pthread_exit(0);

    return (void*)0;
}


int get_mouse_xy(int x, int y, int h, WINDOW *w){

    int x1 = w->pos_x;
    int x2 = w->width;
    int y1 = w->pos_y + 8;
    int y2 = w->height;

    int xy = -1;

    if( x >= x1 && y >= y1 && x <= (x1+x2) && y <= (y1 + y2) ){

        xy = (y - y1)/h;
    }

    return xy;
}

static void fill_menu_data(MENULIST *menu, int count, int xy, int *r_type, WINDOW *w){
    if(!menu)return;

    unsigned int rgb;
    for(int i=0; i < count; i++){
        rgb = w->fg;
        int e = 0;
        if(menu[i].type == 2){
            drawline( 4, 12+(i*24) + (font->y/4), w->width-8, 2, rgb, w);
            continue;
        }
        if(xy == i){
            drawline( 4, 8+(24*i), w->width-8, 24, 0x000022/*rgb*/, w);
            drawrect( 4, 8+(24*i), w->width-8, 24, 0, w);
            rgb = 0xFFFFFF;
            *r_type = menu[i].type;
            menu_message_id = menu[i].id;
            e = 0;
        }else{
            drawline( 4, 8+(24*i), w->width-8, 24, w->bg, w);
        }
        drawstring_trans( menu[i].name, 8 + e, 12+(i*24), rgb, w->bg, font, w);
        if(menu[i].type == 1){
            drawtriagle(w->width-18, 12+(i*24) + (font->y/4), font->y/2, 1, rgb, w);
        }
    }
}

int fill_menu(MENULIST *menu, int count, int x, int y, WINDOW *w){

    if(!menu)return -1;

    int record_type2;
    unsigned int rgb;
    int xy = get_mouse_xy(x, y, 24, w);

    if(xy == -1 && process > 0){
        int xy2 = get_mouse_xy(x, y, 24, w2);
        if(xy2 == -1){
            w2->visibility = 0;
            process = 0;
            menu_message_id = 0;
        }else{
            // process sumen, fill
            if(record_xy2 != xy2){
                int c = global_submenu[0].count;
                fill_menu_data(global_submenu, c, xy2, &record_type2, w2);
                record_xy2 = xy2;
                recorde_xy = -1;
            }
            return 0;
        }
    }

    record_xy2 = 8000;
    process = 1;

    if(xy == -1) menu_message_id = 0;

    if(recorde_xy == xy){
        if(record_type == 1 && w2->visibility == 0){ // submenu
            int c = global_submenu[0].count;
            init_window( w1->pos_x + w1->width,  w1->pos_y + (xy*24), 160, 24*c+16, 0, 0xC0C0C0, 0, w2);
            fill_menu_data(global_submenu, c, -1, &record_type2, w2);
            w2->visibility = 1;
        }
        return 0;
    }

    if(w2->visibility != 0){
        menu_message_id = 0;
        w2->visibility = 0;
    }
    record_type = -1;
    recorde_xy = xy;
    fill_menu_data(menu, count, xy, &record_type, w1);

    return 0;
}

static int commun_switch(struct communication *commun, struct communication *ret, unsigned short udp_port)
{
    unsigned long *message = (unsigned long*)((unsigned long)&commun->message);
    unsigned long new_window = 0;
    switch(commun->type){
        case COMMUN_TYPE_WINDOW_REGISTER:
            new_window = message[0];
            client_listing(new_window, commun->pid, udp_port);
            __asm__ __volatile__("int $0x72"::"d"(2),"D"(new_window), "S"(0));
            return 0;
    }

    return 0;
}


static void server(){
    struct communication *req = (struct communication*) malloc(sizeof(struct communication));
    struct communication *ack = (struct communication*)malloc(sizeof(struct communication));
    int sd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sera;
    struct sockaddr_in serb;

    sera.sin_family         = AF_LOCAL;
    sera.sin_addr.s_addr    = inet_addr("127.0.0.1");
    sera.sin_port           = htons(1025);

    serb.sin_family         = AF_LOCAL;
    serb.sin_addr.s_addr    = htonl(INADDR_ANY);
    serb.sin_port           = htons(INADDR_ANY);

    if(sd < 0) {
        printf("Cannot create socket!\n");
        while(1){}
    }
    int r = bind( sd, (struct sockaddr*)&sera, sizeof(struct sockaddr_in));
    if(r) {
        printf("Cannot bind socket!\n");
        while(1){}
    }

    while(1){
        socklen_t len;
		ssize_t to = recvfrom(sd, req, 100, 0, (struct sockaddr*)&serb, &len);
        unsigned short udp_port = htons(serb.sin_port);
        commun_switch(req, ack, udp_port);
        sendto(sd, ack, sizeof(struct communication), 0, (struct sockaddr*)&serb, sizeof(struct sockaddr_in));
	}

}