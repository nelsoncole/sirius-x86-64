#include <kernel.h>

void kernel_panic(const char *s){
    printf("Kernel panic: %s\n", s);
    while(1)
        __asm__ __volatile__("cli; hlt;");
}
