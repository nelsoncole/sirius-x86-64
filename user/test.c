#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <gui.h>

extern unsigned long __window;

void swap( int *a, int *b)
{
    int save_a = *a;
    int save_b = *b;

    *(int*)(a) = save_b;
    *(int*)(b) = save_a;
}

bool putpixel(int x, int y, unsigned int c )
{
    WINDOW *w = (WINDOW*) __window;
    int width = w->width;
    int height = w->height;
    int bytespp = 4;

    unsigned char *data = (unsigned char *) &w->start;
    
    if(!data || x<0 || y<0 || x>=width || y>=height) {
        return false;
    }

    memcpy(data+(x+y*width)*bytespp, &c, bytespp);
    //put_pixel( x, y, w->width, color, &w->start);
    return true;
}

// Algorítmo de bresenham
void BRESENHAM_line(int x0, int y0, int x1, int y1, unsigned int color )
{
    bool steep = false;
    if(abs(x0-x1) < abs(y0-y1)) {
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = true;        
    }
    
    if(x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = abs(dy)*2; 
    int error = 0;
    int y = y0;
    for(int x=x0; x <= x1; x++) {
        if(steep) {
            putpixel(y, x, color);
        } else {
            putpixel(x, y, color);
        }

        error += derror;
        if( error > dx) {
            y += (y1 > y0?1:-1);
            error -= dx*2;
        }
    }
    

}


// Algorítimo DDA, incrementa qualquer um dos eixos
void DDA_line(int x0, int y0, int x1, int y1, unsigned int color )
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int x = x1;
    int y = y1;
    int p;
    float incx, incy;
    if(abs(dx) > abs(dy)) {
        // deslocamento em x, x recebe incremento unitario
        p = abs(dx);
    }else {
        p = abs(dy);
    }

    incx = dx/(float)p;
    incy = dy/(float)p;

    putpixel(x, y, color);
    for(int i=0; i < p; i++) {
        putpixel(x, y, color);
        x += incx;
        y += incy;
    }
}


extern char* get_ip_from_name(char *addr, const char *name , int query_type);

int main(int argc, char **argv) {

//    DDA_line(400, 100, 500, 300, 0xFFFFFF);

//    DDA_line(400, 100, 600, 300, 0xFFFFFF);
/*    int x = 100;
    int y = 200;
    DDA_line(x + 10, y + 300, x, y + 100, 0xFF0000); */

    if(argc < 2) {
        printf("No address\n");
        return 0;
    }

    char ip[18];
    get_ip_from_name(ip, argv[1], 1);

    printf("IP %s\n", ip);

	return 0;
}
