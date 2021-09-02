#include <io.h>
#include <acpi.h>
#include <stdio.h>
#include <string.h>

#include <gui.h>
#include <pci.h>
#include <cpuid.h>


#define SHELL_CMD_NUM 18

void (*call_loader)  (int argc,char **argv) = 0;

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
    	{"shutdown",    cmd_shutdown,       "Shutdown your computer locally or remotely"    	},
   	{"time",        cmd_time,           "Time"                                          	},
    	{"version",     cmd_version,        "Shell version"                                 	},
};



void console() {
	
	int of = 0;
	char ch = 0;
	char argv[256];
	memset(argv,0,256);
	
	
	printf("\n\n~ $ ");
	
	while(1) {
	
		ch = fgetc(stdin);

		if(ch != 0)
		{
			cli();
			printf("%c",ch);
			sti();
			if(ch == '\n')
			{
				if(of != 0)
				for(int i=0;i < SHELL_CMD_NUM;i++) {

            				if((strcmp(argv ,cmd_table[i].name)) == 0) {

						call_loader = (void(*)(int,char **))(cmd_table[i].fun);
						call_loader(1, (char**)argv);
                				break;
            				}
            				else if(i == (SHELL_CMD_NUM - 1)) {
            					cli();
            					printf("%s: command not found\n",argv);
            					sti();

					}
			
        			}
        			
        			memset(argv,0,256);
				of = 0;
				cli();
				printf("~ $ ");
				sti();
			
			}else { 
			
				if(ch == '\b' && of > 0) argv[--of] = 0;
				else if(ch == '\b') argv[of] =  ch;
				else argv[of++] =  ch;
				
			}
			
			
			
			
		}
		
		__asm__ __volatile__("hlt");
	
	}	

}

int cmd_version(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_time(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_shutdown(int argc,char **argv)
{
	cli();
	poweroff();
	sti();
	return 0;
}
int cmd_rename(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_reboot(int argc,char **argv)
{
	cli();
    	reboot();
    	sti();
	return 0;
}
int cmd_new(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_mov(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_info(int argc,char **argv)
{
	char cpu_name[128];
	memset(cpu_name,0,128);
	
	cpuid_processor_brand(cpu_name);
		
	cli();
	printf("Sirius OS (Kernel mode: AMD64 or x86_64)\nCPU: %s\n",cpu_name);
	pci_get_info(0,2);
	sti();
	return 0;
}
int cmd_help(int argc,char **argv)
{
	int i,j;
	cli();
      	puts("Commands:\n");
      	for(i=0;i< SHELL_CMD_NUM;i++){
        	puts(cmd_table[i].name);
        	//set_cursor
		for(j=strlen(cmd_table[i].name);j < 18; j++) putchar(' ');

        	puts(cmd_table[i].help);
		putchar('\n');

        }
	sti();
	return 0;
}
int cmd_exit(int argc,char **argv)
{	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_echo(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_dir(int argc,char **argv)
{	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_del(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_date(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_copy(int argc,char **argv)
{	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
int cmd_clear(int argc,char **argv)
{	cli();
	clears_creen();
	sti();
	return 0;
}
int cmd_cd(int argc,char **argv)
{
	cli();
	puts("Function not implemented\n");
	sti();
	return 0;
}
