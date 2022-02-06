#include <stdio.h>
#include <string.h>
#include <kernel.h>

void kernel_panic(const char *fmt, ...){

    char buf[256];
	memset(buf,0,256); 

  	va_list ap;
  	va_start (ap, fmt);
  	vsprintf(buf, fmt, ap);
  	va_end (ap);
  	
    printf("kernel panic: ");
  	puts(buf);
  	

    while(1){__asm__ __volatile__("cli; hlt;");}
}
