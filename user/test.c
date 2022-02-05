#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <gui.h>

#include <netinet/in.h>
#include <arpa/inet.h>

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
char bf[4096];
int main(int argc, char **argv) {

//    DDA_line(400, 100, 500, 300, 0xFFFFFF);

//    DDA_line(400, 100, 600, 300, 0xFFFFFF);
/*    int x = 100;
    int y = 200;
    DDA_line(x + 10, y + 300, x, y + 100, 0xFF0000); */

    /*
    if(argc < 2) {
        printf("No address\n");
        return 0;
    }*/

    if(argc < 2) {
        printf("No IP...\n");
        return 0;
    }

    if(argc < 3) {
        printf("Port Null...\n");
        return 0;
    }

    int dns = 0;
    if(argc < 4) {
        dns = 1;
    }

    int port = strtoul (argv[2],NULL, 10);
    char ip[128];
    if(!dns)
        strcpy(ip, argv[1]);
    else
    if(!get_ip_from_name(ip, argv[1], 1)){
        printf("DNS No IP...\n");
        return 0;
    }

    printf("IP: %s, Port: %d\n", ip, port);

    int client;
    struct sockaddr_in  saddr;

    saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //printf("Socket criado\n");
    
    if( connect(client, (struct sockaddr*)&saddr, sizeof(saddr)) ){
	    printf("Erro na concaxao\n");
	    shutdown(client, 1);
	    return (0);
    }
    //printf("Conetado ao servidor\n");

    int y = 0;
    while(1){
        //
        memset(bf, 0, 1024);
        printf("< ");
        int r = recv(client, bf, 1024, 0);
        printf("%s\n",bf);
    
        if(y >= 1 || dns == 0 ) {

            printf("> ");
            fgets(bf,1024,stdin);
            send(client, bf, strlen(bf),0);
        }

        y++;
    }
    shutdown(client, 0);
	return 0;
}



