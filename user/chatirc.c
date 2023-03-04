#include <window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void main(int argc, char **argv){

    WINDOW *w = get_window_surface();
    init_window( 0, 0, 400, 400, rgb(6,120,181), 0xC0C0C0, WINDOW_STYLE_DEFAULT, w);
    __wcl(w);

    while(1){

    }
}