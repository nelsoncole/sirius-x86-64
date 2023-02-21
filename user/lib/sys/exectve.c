#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/communication.h>
#include <sys/exectve.h>


extern char *pwd;
#define COUNT_ARGV 32

int exectve(int argc,char **argv, char type, unsigned long app_id)
{
    if(argc < 1) return - 1; 
    char *path;
    char *app_memory;
    int ret = 0;
    struct communication commun, commun2;
    memset((char*)&commun, 0, sizeof(struct communication));
    switch(type){
        case 0:
            commun.type = COMMUN_TYPE_EXEC;
            commun.apid = app_id;
            break;
        case 1:
            commun.type = COMMUN_TYPE_EXEC_CHILD;
            unsigned long pid = 0;
            // getpid()
            __asm__ __volatile__("int $0x72":"=a"(pid):"d"(1),"c"(0));
     
            commun.pid = pid;
            break;
        default:
            return -1;
    }
	
	path = malloc(128);
	char *v = (char*) malloc(COUNT_ARGV*8 + 256*COUNT_ARGV + 8);
    char *tmp = v;
	for(int i=0; i < argc; i ++){
		strcpy(v,argv[i]);
        tmp += strlen(argv[i]);
        *tmp++ = ' ';
     	
	}
    tmp--;
    *tmp = '\0';
	
    strcpy(path, argv[0]);
    FILE *fp = fopen(path,"r+b");
    if(fp) {
        app_memory = (char*)malloc(0x80000); // 512
        fseek(fp,0,SEEK_END);
	    int size = ftell(fp);
	    rewind(fp);
        fread (app_memory, 1, size, fp);
	    fclose(fp);

        unsigned long *addr = (unsigned long*)((unsigned long)&commun.message);
        addr[0] = (unsigned long)((unsigned long*)app_memory);
        strcpy( (char*)&addr[1],pwd);
        char *arg = (char*)&addr[1];
        arg += strlen(pwd) + 1;
        strcpy( arg, (const char*)v);
        communication(&commun, &commun2);

		if(commun2.type)
		{
            printf("executive error: \'%s\'\n", path);
        }

        free(app_memory);

    }else {
        printf("fopen error: \'%s\'\n", path);
        ret = -1;
	}

    free(v);
	free(path);
	return ret;
}