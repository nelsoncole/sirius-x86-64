#include <netinet/in.h>

unsigned short ether_getport(){
    unsigned short r = 0;
    __asm__ __volatile__("int $0x72;":"=a"(r):"d"(27));
    return r;
}