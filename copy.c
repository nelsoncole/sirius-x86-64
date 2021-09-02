#include <stdio.h>
#include <stdlib.h>

// copiar 8MiB
int main(int argc, char **argv)
{
	if(argc < 3) return 0;

	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"r+b");
	
	if(src == 0 || dst == 0) { 
		printf("error\n");
		exit(1);
	} 
	
	char *data = malloc(4096);
	
	for(int i=0; i < (1024*2); i ++) {
	
		fread(data, 1, 4096, src);
		fwrite(data, 1, 4096, dst);
		fflush(dst);
	
	}
	
	fclose(dst);
	fclose(src);
	free(data);
	printf("done\n");
	
	exit(0);
	return 0;
}
