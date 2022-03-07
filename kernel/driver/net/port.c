#include <ethernet.h>
#include <mm.h>
#include <string.h>
unsigned char *port_bitmap;

void ethernet_port_setup(){

    unsigned long addr = 0;
    // 8 KiB
    alloc_pages(0, 2, (unsigned long*)&addr);

    port_bitmap = (unsigned char*)addr;

    memset(port_bitmap,0,2*4096);

    set_port(0);
}

unsigned short get_port(){
    unsigned int port = 0;
    unsigned char *bmp = port_bitmap;
    for( int i = 0; i < (65536/8); i++){
        for(int b =0; b < 8; b++){
            if(!(*bmp & (1<<b))) {
                *bmp |= (1<<b); 
                goto end;
            }
            port++;
        }
        bmp++;
    }

end:
    if(port >= 65536) return 0;

    return (unsigned short)port;
}

unsigned short set_port(unsigned short port){

    unsigned char *bmp = port_bitmap;
    bmp += port/8;
    int b = port%8;
    
    if((*bmp & (1<<b))) {
        return 0;
    }
 
    *bmp |= (1 << b);

    return port;   
}

int clean_port(unsigned short port){

    unsigned char *bmp = port_bitmap;
    bmp += port/8;
    int b = port%8;
 
    *bmp &=~(1<<b); 
   
    return 0;   
}
