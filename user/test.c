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
void line(int x0, int y0, int x1, int y1, unsigned int color )
{
    int a, b;
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

void triangle(int t0, int t1, int t2, unsigned int c)
{/*
    line(int x0, int y0, int x1, int y1, c );
    line(int x0, int y0, int x1, int y1, c );
    line(int x0, int y0, int x1, int y1, c );*/
}

int main(int argc, char **argv) {

	/*unsigned char *a = (unsigned char*) malloc(0x100000); //1MiB
	int size = 0;
	FILE *f = fopen("ubuntu.ttf","r");
	if(f == NULL) printf("error: fopen: ubuntu.ttf\n");
	else {
	
		fseek(f, 0,SEEK_END);
		size = (int) ftell(f);
		fseek(f, 0,SEEK_SET);
		
		if(size < 0x100000) {
			if(fread(a, 1, size, f) != size) printf("error: fread\n");	
		}
	
		printf("TTF: filesize: %d\n",size);
		
		for(int i=0; i < 0x200; i ++) putchar(a[i]);
	
		putchar('\n');
	}
	
	free(a);
	fclose(f);*/
    
	/*line(13, 60, 80, 40, 0xffffff);
    line(20, 13, 40, 80, 0xff0000);
    line(80, 40, 13, 20, 0xff0000);*/

    int x = 100;
    int y = 200;

    /*line( x + 25, y , x, y - 100, 0xFF0000);
    line( x - 25, y , x, y - 100, 0xFF0000);
    line( x, y , x, y - 100, 0xFF0000);*/



    double r = 100;
    for(int i=0; i < 100; i++){
        //line( 100, 200 , 100, 100, 0xFF0000);
        line( 100, 200 , 100+i, r, 0xFF0000);

       // r = sqrt();
    }

	return 0;
}
