#include <string.h>
#include <stdlib.h>
extern char *pwd;


int setpath(const char *name)
{

	char *p = pwd;
	char *dst = (char*)name;
	int len1 = strlen(p);
	int len2 = strlen(dst);
	
	if((len1 + len2) > 0x1000) 
		return -1;
		
	if(*dst == '/')
		dst ++;
	
	p += len1;
	p --;
	if(*p != '/') {
		p ++;
		*p ++ = '/';
	}else p ++;
	
	strcpy(p, dst);
		 
	p += strlen(dst);
	p --;
	if(*p == '/') *p = '\0';
	else p ++;
	
	*p = '\0';	
	return 0;
}

int upath()
{

	if(strlen(pwd) < 3)
		return 0;

	char *p = pwd;
	p += strlen(p);
	
	while( *p != '/') {
	
		*p -- = '\0';
	}
	
	if(strlen(pwd) > 2) {
		*p = '\0';
	}
	
	return 0;
}


char *getpathname(char *dst, const char *src )
{
	char *usrc = (char *) src;
	char *udst = (char *) dst;
	
	
	if(src == 0 || dst == 0) return 0;
	
	int len = strlen(usrc);
	
	if( *(char*)(usrc + len -1) == '/' )
		*(char*)(usrc + len -1) = '\0';
	
	if(len < 1) return 0;
	
	if(len > 1) {
		if( usrc[1] == ':' )
			strcpy(udst, usrc); //raiz
		else {
			strcpy(udst, getenv("PWD"));
			len = strlen(udst);
			if(*usrc != '/') {
				*(char*)(udst + len ) = '/';
				len ++;
			}
			
			strcpy(udst + len, usrc);
			
		}
			
	}else {
		strcpy(udst, getenv("PWD"));	
		len = strlen(udst);
		if(*usrc != '/') {
				*(char*)(udst + len ) = '/';
				len ++;
		}
		strcpy(udst + len, usrc);
	}

	return udst;
}

int path_count(const char *path)
{

	unsigned char *p = (unsigned char *)path;
	
	if( !p ) return 0;
	
	int i =0;
	
	while(*p != '\0') {
	
		if( *p == '/') i ++;
		p ++;
	}
		
	return i;
}

int getfilename(char *filenane, char *path){

	char *p = (char *)path;
	char *f = (char *)filenane;
	
	if( !p || !f) return -1;
	
	int len = strlen(p);

	p += len;	
	p --;
	while(*p && len-- > 0) {
		if( *p == '/') break;
		p --;
	}
	
	p ++;
	strcpy(f,p);
	
	*p ='\0';	

	return 0;
}


