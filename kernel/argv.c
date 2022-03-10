#include <string.h>

static int cpy(char *dst, const char *src){
    
    if(*src == '\0') return 0;
    
    char flag = 0;
    int i = 0;
    for(i=0; i < 256; i++){
        char val = src[i];
        if(val != ' '){
            if(flag > 0) break;

            dst[i] = val;

            if(val == '\0') break;
        }
        else{
            flag = 1;
            dst[i] = '\0';
        }
    }

    return i;
}

int format_argv(int *argc, char **argv, char *src){

    char *src_tmp = src;
    char tmp[256]; 
    unsigned long mem = (unsigned long)argv;
    mem += 8*32;

    int i = 0;
    for(i =0; i < 32; i++){
        argv[i] = (char*)mem;
        int r = cpy(tmp, (const char *)src_tmp);
        if(r == 0) break;
        strcpy(argv[i], tmp);
        mem += r;
        src_tmp += r;
        
    }
    *argc = i;
    return i;

}
