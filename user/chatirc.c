#include <window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/communication.h>

void main(int argc, char **argv){

    struct con con[1];
    con_init(con);

    WINDOW *w = get_window_surface();
    init_window( 0, 0, 400, 400, rgb(6,120,181), 0xC0C0C0, WINDOW_STYLE_DEFAULT, w);

    create_button("Enviar", 160, 200, 80, 40, 0, 0xe0e0e0, w, 100);
    create_textbox("", 4, 50, 200, 30, 0, 0xffffff, w, 101);
    __wcl(w, con);

    while(1){
        con_recvfrom(con);
        switch(con->ack.type){
            case 100:
                //button envir
            break;
            case 101:
                // textbox
            break;
        }
    }
}