#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if(argc < 3)
    {
        printf("Footer VHD falhu!\n");
        return 0;
    }

	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"r+b");
	
	if(src == 0 || dst == 0) { 
		printf("Footer VHD error!\n");
		exit(1);
	} 
	
	char *data = malloc(512);
	
	fread(data, 1, 512, src);
    fseek(dst,512,SEEK_END);
	fwrite(data, 1, 512, dst);

	fflush(dst);
	fclose(dst);
	fclose(src);
	free(data);
	printf("done!\n");
	
	exit(0);
	return 0;
}
