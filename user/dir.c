#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <window.h>
#include <file.h>


int dir(){

 directory_entry_t *d = (directory_entry_t *) malloc(0x2000);
 memset(d, 0, 0x2000);

 int count = open_dir("./", d, 64);
 
 if( count == -1) {
 
 	//printf("No such file or directory")
 	printf("dir: error\n");
 	free(d);
 	return 0;
 }else if (!count) {
 
 	printf("dir: empty\n");
 	free(d);
 	return 0;
 }
 
 for(int i=0;i < count; i ++){
 	
 	if(d->attr&0x40) {
 		color(0xFFFF00);
 		printf("%s\n", d->file_name);
 		color(0xFFFFFF);
 	}else
 		printf("%s\n", d->file_name);
 	
 	d ++;
 }
 
 free(d);
 return 0;
}
