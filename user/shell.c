#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <window.h>

#include <pipe.h>
#include <file.h>
#include <path.h>

extern void term_color(int fg, int bg);

void shell();
int main()
{
	
	shell();
	return 0;
}


#define SHELL_CMD_NUM 18
#define COUNT_ARGV 32

unsigned long argv_pointer[COUNT_ARGV];
unsigned long _v_;
char *app_memory;
void (*call_loader)  (int argc,char **argv);
extern char *pwd;
extern int call_function(void *addr,int argc,char *argv[]);

typedef struct _COMMAND {
  	char *name;
  	void *fun;
  	char *help;
}__attribute__((packed)) COMMAND;

int cmd_version(int argc,char **argv);
int cmd_time(int argc,char **argv);
int cmd_shutdown(int argc,char **argv);
int cmd_rename(int argc,char **argv);
int cmd_reboot(int argc,char **argv);
int cmd_new(int argc,char **argv);
int cmd_mov(int argc,char **argv);
int cmd_info(int argc,char **argv);
int cmd_help(int argc,char **argv);
int cmd_exit(int argc,char **argv);
int cmd_echo(int argc,char **argv);
int cmd_dir(int argc,char **argv);
int cmd_del(int argc,char **argv);
int cmd_date(int argc,char **argv);
int cmd_copy(int argc,char **argv);
int cmd_clear(int argc,char **argv);
int cmd_cd(int argc,char **argv);

COMMAND cmd_table[] = {
    {"?",           cmd_help,           "This help"                                     	},
    {"cd",          cmd_cd,             "Change current directory"                      	},
    {"clear",       cmd_clear,          "Clear screen"                                  	},
    {"copy",        cmd_copy,           "Copy file or directory"                        	},
    {"date",        cmd_date,           "Date"                                          	},
    {"del",         cmd_del,            "Delete file or directory"                      	},
    {"dir",         cmd_dir,            "List directory"                                	},
    {"echo",        cmd_echo,           "This ---"                                      	},
    {"exit",        cmd_exit,           "Exit shell"                                    	},
    {"help",        cmd_help,           "This help"                             			},
	{"info",        cmd_info,           "This System information"   		   		},
    {"mov",         cmd_mov,            "Move file or directory"                        	},
    {"new",         cmd_new,            "New file or directory"                         	},
    {"reboot",      cmd_reboot,         "Reboot system"                                 	},
    {"rename",      cmd_rename,         "Rename file or directory"                      	},
    {"poweroff",    cmd_shutdown,       "Turn off the computer"    				},
   	{"time",        cmd_time,           "Time"                                          	},
    {"version",	cmd_version,	     "Shell version"                                		},
};


#define DEL "\t "

void wait_exit(){

	__pipe_t pipe[1];
	memset(pipe,0 , 16);
	
	for(;;){
	
	 pipe_read ( pipe, __pipe__);
	 if(pipe->id == 0x1001)  break;
	
	}

}

char *path;

static int cmd_run(int argc,char **argv)
{

	if(argc < 1) return - 1; 
 

	//strcpy(path,"hello.bin\0");
	char *a = argv[0];
	a +=2;
	
	if(*a == '0') return - 1;
	
	
	char *v = (char*) _v_;
	memset(v, 0, COUNT_ARGV*8 + 256*COUNT_ARGV + 8);
	*(unsigned long*)v = argc;
	v += 8;
	v += 8*COUNT_ARGV;
	for(int i=0; i < argc; i ++){
	
		strcpy(v,argv[i]);
		
		v += 256;
	}
	
    strcpy(path, a);
    FILE *fp = fopen(path,"r+b");
    if(fp) {
        fseek(fp,0,SEEK_END);
	    int size = ftell(fp);
	    rewind(fp);
        fread (app_memory, 1, size, fp);
	    fclose(fp);

        __asm__ __volatile__("movq %%rax,%%r8;"
	    " int $0x72;"
	    ::"d"(8),"D"(app_memory),"S"(_v_),"c"(1), "a"(pwd)); // fork

        wait_exit();

    }else {
        printf("fopen error: \'%s\'\n", path);
	}

		
	return 0;
}

int set_argv(char *s)
{
	int i = 0;
	
	unsigned char *us = (unsigned char *)s;

	for( ;i < COUNT_ARGV ;){
	
		
		argv_pointer[i ++] = (unsigned long)us;
		
		while(*us != '\0' && *us != ' ') us ++;
		
		if(*us == '\0') break;
		
		*us ++ = '\0';
		
		while(*us == ' ') us ++;
		
		if(*us == '\0') break;
		
	}

	return i;
}

void shell() {

    app_memory = (char*)malloc(0x80000); // 512
	
	int argc = 0;
	char *argv = (char*) malloc(0x1000);
	char *name = (char*) malloc(0x1000);
	path = malloc(128);
	_v_ = (unsigned long)malloc(COUNT_ARGV*8 + 256*COUNT_ARGV + 8);
	
	while(1) 		{
	
		memset((char*)argv_pointer, 0, sizeof(unsigned long)*COUNT_ARGV);
		memset(argv,0,0x1000);
		sprintf(name, "<%s> ~ $",getenv("PWD"));
        term_color(2, -1);
		printf("%s ", name);
		term_color(-1, -1);
		fgets (argv,0x1000,stdin);
        // limpar o '\n'
        unsigned char *clean = (unsigned char*)argv;
        clean += strlen(argv) - 1;
        *clean = '\0';
		
		//argv = strtok(argv,DEL);
		//argv_pointer[0] = (unsigned long)argv;
		argc = set_argv(argv);
		
		char *cmd = (char*)argv_pointer[0];
		

		for(int i=0;i < SHELL_CMD_NUM;i++) 	{
			
			
			if(argv != 0)	{
            			if((strcmp(cmd,cmd_table[i].name)) == 0) 	{
					call_loader = (void(*)(int,char **))(cmd_table[i].fun);
					call_loader(argc, (char**) argv_pointer);
					//call_function(cmd_table[i].fun, argc, (char**) argv_pointer);
                			break;
            			}
            			else if(i == (SHELL_CMD_NUM - 1)) 	{
            				printf("%s: command not found\n", (char*)cmd);
            			}else {
            				if((strncmp(cmd,"./",2)) == 0 ) {
        					call_loader = (void(*)(int,char **))(cmd_run);
						call_loader(argc, (char**) argv_pointer);
						//call_function(cmd_run, argc, (char**) argv_pointer);
						break;
        				}
            			
            			}
            		}
			
        	}
        	
        			
	}
	
	free(path);
	free(name);
	free(argv);
	exit(0);       	

}

int cmd_version(int argc,char **argv)
{
	puts("Function not implemented\n");
	return 0;
}
int cmd_time(int argc,char **argv)
{
	puts("Function not implemented\n");
	return 0;
}
int cmd_shutdown(int argc,char **argv)
{
	__asm__ __volatile__("int $0x72"::"d"(8),"c"(4));
	return 0;
}
int cmd_rename(int argc,char **argv)
{
	if(argc > 2) 
		if( rename(argv[1], argv[2]) == 0 ) 
			return 0;
		
	printf("rename: error\n");
	return -1;
}
int cmd_reboot(int argc,char **argv)
{
    	__asm__ __volatile__("int $0x72"::"d"(8),"c"(3));
   	return 0;
}
int cmd_new(int argc,char **argv)
{

	if(argc < 2) return -1;

	FILE *f = fopen(argv[1], "w");
	if(!f) return -1;

	fclose(f);

	return 0;
}
int cmd_mov(int argc,char **argv)
{
	puts("Function not implemented\n");

	return 0;
}
int cmd_info(int argc,char **argv)
{
	char cpu_name[128];
	memset(cpu_name,0,128);
	
	//cpuid_processor_brand(cpu_name);
	
	printf("Sirius OS (Kernel mode: AMD64 or x86_64)\nCPU: %s\n",cpu_name);
	
	return 0;
}
int cmd_help(int argc,char **argv)
{
	int i,j;
      	puts("Commands:\n");
      	for(i=0;i< SHELL_CMD_NUM;i++){
        	puts(cmd_table[i].name);
        	//set_cursor
		for(j=strlen(cmd_table[i].name);j < 18; j++) putchar(' ');

        	puts(cmd_table[i].help);
		putchar('\n');

        }

	return 0;
}
int cmd_exit(int argc,char **argv)
{
	//puts("Function not implemented\n");
	//exit(0);

	return 0;
}
int cmd_echo(int argc,char **argv)
{
	puts("Function not implemented\n");

	return 0;
}

extern int dir();
int cmd_dir(int argc,char **argv)
{
	dir();

	return 0;
}
int cmd_del(int argc,char **argv)
{
	if(argc < 2) {
		printf("Erro, arquivo ou directorio nao encontrado\n"); 
		return -1;
	}
	if(remove (argv[1]) != 0) {
		printf("Erro, arquivo ou directorio nao encontrado\n"); 
		return -1;
	}
	return 0;
}
int cmd_date(int argc,char **argv)
{
	puts("Function not implemented\n");

	return 0;
}
int cmd_copy(int argc,char **argv)
{	
	puts("Function not implemented\n");
	
	return 0;
}

int cmd_clear(int argc,char **argv)
{	
	WINDOW *w = (WINDOW*) __window;
	__window_clear(w);
	return 0;
}
int cmd_cd(int argc,char **argv)
{

	if(argc < 2) return 0;
	

	if( strcmp(argv[1],"..") == 0 ) { 
		upath();
		return 0;
	}
	
	directory_entry_t *d = (directory_entry_t *) malloc(0x2000);
 	memset(d, 0, 0x2000);
	char *path = (char *)malloc(0x1000);
	sprintf(path,"%s/.",argv[1]);
	
	int r = open_dir(path, d, 64);
	
	free(d);
	free(path);
	
 	if( r == -1) {
		printf("cd: %s: No such file or directory\n", argv[1] );
 		return -1;
 }
	
	if( setpath(argv[1]) ) {
		printf("cd: %s: No such file or directory\n", argv[1] );
		return -1;
	}
	
	return 0;
}
