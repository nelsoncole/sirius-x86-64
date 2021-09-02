#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gui.h>
#include <fs.h>


int putchar(int c)
{
	return (glyph(c));

}

void puts(const char* str)
{
	
	int i;
	if(!str)return;
	 
	for(i=0;i <strlen(str);i++) putchar(str[i]);
}

int printf(const char *fmt, ...)
{
	char buf[256];
	memset(buf,0,256); 

	int ret;
  	va_list ap;
  	va_start (ap, fmt);
  	ret = vsprintf(buf, fmt, ap);
  	va_end (ap);
  	
  	puts(buf);
  	
  	return ret;

}

//FILE
/*
 *  Modo 	Significado
 *
 * r		Abre um arquivo-texto para leitura
 * w		Cria um arquivo-texto para escrita
 * a		Anexa a um arquivo-texto
 * rb		Abre um arquivo binário para leitura
 * wb		Cria um arquivo binário para escrita
 * ab		Anexa a um arquivo binário
 * r+		Abre um arquivo-texto para leitura/escrita
 * w+		Cria um arquivo-texto para leitura/escrita
 * a+		Anexa ou cria um arquivo-texto para leitura/escrita
 * r+b		Abre um arquivo binário para leitura/escrita
 * w+b		Cria um arquivo binário para escrita/escrita
 * a+b		Anexa ou cria um arquivo binário para escrita/escrita
 *
 *	
 */


FILE *fopen (const char *path,const char *mode) 
{
	if(mode[0] == '\0') return NULL;


	FILE *fp = open_file(path,mode);

	return fp;
	
}

int fclose(FILE *fp)
{
	return file_close(fp);
}

void rewind (FILE *fp)
{
	if(!fp) return;
	
	fp->off = fp->off2 = 0;
	fp->level = 0;
	
	// define leitura
	fp->flags &=~0x10;

}


int fseek (FILE *fp, long num_bytes, int origin )
{	
	switch(origin) {

		case SEEK_SET:

			fp->off = num_bytes;
			fp->off2 = num_bytes;
		break;
		case SEEK_CUR:

			fp->off += num_bytes;
			fp->off2 += num_bytes;
		break;
		case SEEK_END:
			
			fp->off = fp->fsize - num_bytes;
			fp->off2 = fp->off;
		break;

	}

	fp->level = fp->off % fp->bsize;

	return 0;
}

long int ftell(FILE *fp)
{
	if(!fp) return (0);

	return (fp->off);

}

int fgetc (FILE *fp)
{	
	int r;
	
	if(fp->off >= fp->fsize) {
	
		return EOF;
	}
	
	
	if(!(fp->flags&0x10)) {
		// ler
		fp->flags |= 0x10;
		
		int addr = fp->off/fp->bsize;
		file_read_block(fp, addr);
	}


	fp->curp = (unsigned char*)(fp->buffer + fp->level++);
	r = *(unsigned char*)(fp->curp) &0xff;

	// update
	if(fp->level >= fp->bsize) {
		fp->flags &=~0x10;
		fp->level = 0;
	}
	
	fp->off++;

	return (r);
}

size_t fread (void *buffer,size_t size, size_t count, FILE *fp)
{
	if(!fp) return (0);

	size_t i;
	int c;
	size_t rc = 0;

	unsigned char *buf = (unsigned char*)buffer;

	for(i=0;i < size*count;i++)
	{
		c = fgetc (fp);
			//feof(fp);
		if(c == EOF) return (rc/size);

		*buf++ = c;
		rc++;
	}

	return (rc/size);
}


