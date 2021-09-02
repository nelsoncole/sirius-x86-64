#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <setjmp.h>

jmp_buf env;

void segunda()
{
    printf("segunda\n"); // será impresso na tela
    // pula de volta para onde setjmp() foi chamada
    // e faz com que ela agora retorne 1
    longjmp(env, 1);
}

void primeira()
{
    segunda();
    printf("primeira\n"); // não será impresso na tela
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
    
	if(setjmp(env) == 0) {
        primeira();
    } else {
        // quando longjmp() é chamada, setjmp() retornou 1
        printf("main\n"); // será impresso na tela
    }
	return 0;
}
