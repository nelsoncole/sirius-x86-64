#ifndef __STDIO_H
#define __STDIO_H

#include "ctype.h"
#include "size_t.h"
#include <stdarg.h>


#define NULL ((void *)0)

#define EOF (-1)
#define _IOFBF		0
#define _IOLBF		1
#define _IONBF		2

#define BUFSIZ		8192

#define FOPEN_MAX	16
#define FILENAME_MAX	96

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2


#define L_tmpnam FILENAME_MAX


// Virtual File System
/*
 *
 * Flags
 * 0x2  =stdin
 * 0x3  =stdout
 * 0x4  =stderr 
 * 0x10 = Leitura
 * 0x20 = Arquivo
 * 0x40 = Pronto
 * 0x80 = File null
 * 0x88 = Spin lock
 * 	
 */
typedef struct _FILE {
	short 		level;		// Nível do buffer cheio/vazio
	unsigned 	flags;		// Sinalizadores de status
	char 		fd;		// Descritor de arquivo
	unsigned char 	hold;		// Caractere ungetc se não existir um buffer
	unsigned 	bsize;		// Tamanho do buffer
	unsigned char 	*buffer;	// Buffer de transferência
	unsigned char 	*curp;		// Ponteiro activo atual
	unsigned 	istemp;	// Indicador de arquivo temporário
	short 		token;		// Usado para verificação de validade
	//
	unsigned char 	mode;		// 0x1 r, 0x2 r/w
	unsigned int	off;
	unsigned int	off2;
	//
	char 		fname[256];	// File name
	unsigned int 	fsize;		// File Size
	unsigned int 	byte_per_sector;
	unsigned int 	count;
	unsigned int	fs_type;	// Tipo de sistema de arquivo
	unsigned int 	num_of_blocks;
	
	
	// directory entry
	unsigned int	dir_cluster;
	unsigned int	dir_entry;
	
	 
	// 
	unsigned int 	dv_num;
	unsigned long 	*blocks;	// 1024*1024 (absolute address sector)


}__attribute__ ((packed)) FILE;

// Standard streams.
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

// C89/C99 say they're macros.  Make them happy.
#define stdin stdin
#define stdout stdout
#define stderr stderr


// E/S de arquivo
extern FILE *fopen(const char *filename,const char *mode);
extern int fclose (FILE *fp);
extern int fflush(FILE *fp);


// caracters
extern int putc (int ch, FILE *fp);
extern int getc (FILE *fp);
extern int fputc (int ch, FILE *fp);
extern int fgetc (FILE *fp);


// string
extern char *fgets (char *str,int length,FILE *fp);
extern int fputs (const char *str,FILE *fp);

// outras
extern size_t fread (void *buffer, size_t size, size_t count, FILE *fp);
extern size_t fwrite (const void *buffer, size_t size, size_t count, FILE *fp);

extern int remove (const char *path);
extern void rewind(FILE *fp);

extern int fseek (FILE *fp, long num_bytes, int origin );
extern int feof (FILE *fp);
extern long int ftell(FILE *fp);


extern int vfprintf(FILE * fp,const char * fmt, va_list ap);
extern int fprintf(FILE *fp, const char *fmt, ...);

extern int vfscanf(FILE * fp,const char * fmt, va_list ap);
extern int fscanf(FILE * fp, const char * fmt, ...);


// E/S de string
extern int vsprintf(char * str,const char * fmt, va_list ap);
extern int sprintf(char * str, const char * fmt, ...);

extern int snprintf(char *s, size_t n,const char * fmt, ...);
extern int vsnprintf(char *s, size_t n,const char *fmt, va_list ap);


// E/S pelo console
extern int putchar(int c);
extern void puts(const char* str);
extern int printf(const char *format,...);

extern int scanf(char *fmt, ...);
extern int getchar ();

extern int ungetc(int c, FILE *fp);

extern char *tmpnam(char *s);
extern int rename(const char *old, const char *new);
extern void perror(const char *s);


// FIXME sem suporte
extern int sscanf(const char *s,const char *fmt, ...);
extern FILE *freopen(const char *filename, const char *mode, FILE *fp);





void debug_o(char *s, int a, int b );




#endif
