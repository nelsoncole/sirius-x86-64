#include <io.h>

void wait(unsigned int n){

	unsigned int ns = n;
	
	while(ns--) outanyb(0x80);
}
