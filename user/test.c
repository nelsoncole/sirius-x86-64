#include <stdio.h>
#include <math.h>

#include <window.h>

// NICK Nelson
// PING :36D9E40F
// PONG 36D9E40F
// USER Nelson 0 \* : Nelson Cole
// PRIVMSG #KernelDev : Olá Lucky

void circumference(int Xc, int Yc, int r, int color)
{
    //printf("Equacao Parametrica\n");
    
    WINDOW *w = (WINDOW*) (__window);

    int x, y, t;

    x = Xc + r;
    y = Yc;
    
    for(t = 1; t <= 360; t++)
    {
        put_pixel( x, y, w->width, color, &w->start);
        x = (int)(Xc + r * cos((3.14 * t) / 180));
        y = (int)(Yc + r * sin((3.14 * t) / 180));
    }

}

void circumference2(int Xc, int Yc, int r, int color)
{
    //printf("Algoritmo de Bresenham\n");

    WINDOW *w = (WINDOW*) (__window);

    int x, y, p;

    x = 0;
    y = r;
    p = 1 - r; // parametro de decisao
    
    while(x <= y)
    {
        // octante
        put_pixel(  x + Xc,  y + Yc, w->width, color, &w->start);
        put_pixel(  y + Xc,  x + Yc, w->width, color, &w->start);
        put_pixel( -y + Xc,  x + Yc, w->width, color, &w->start);
        put_pixel( -x + Xc,  y + Yc, w->width, color, &w->start);
        put_pixel( -x + Xc, -y + Yc, w->width, color, &w->start);
        put_pixel( -y + Xc, -x + Yc, w->width, color, &w->start);
        put_pixel(  y + Xc, -x + Yc, w->width, color, &w->start);
        put_pixel(  x + Xc, -y + Yc, w->width, color, &w->start);

        if(p >= 0)
        {
            y -= 1; // inverte sinal de y
            p = p + 2 * x - 2 * y + 5;
            x = x + 1;
        }
        else
        {
            p = p + 2 * x + 3;
            x = x + 1;
        }
       
    }

}

int main(int argc, char **argv) {

    printf("Equacao Parametrica\n");
    printf("Algoritmo de Bresenham\n");

    circumference(150, 300, 100, 0xFF00);
    circumference2(450, 300, 100, 0xFF00);
    

	return 0;
}



