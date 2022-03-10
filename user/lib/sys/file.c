#include <stdio.h>
#include <file.h>

extern int term_fg_color;
extern int term_bg_color;

int stdputc_r (int c, FILE *fp)
{
	
	if(fp->level >= fp->bsize || fp->off >= fp->bsize) {

		fp->off = 0;
		fp->level = 0;
	}
	
	
	if(fp->flags == 4/*stderr*/)
		stdputc_r (c,stdout);

	if(fp->flags == 3/*stdout*/) {

        unsigned short val = c &0xff;

        int fg = term_fg_color;
        int bg = term_bg_color;
        if(bg > 15 || bg < 0) bg = 0;
        if(fg > 15 || fg < 0) fg = 0;
        if(fg == 15 && bg == 15) bg = fg = 0;

        unsigned char clor = ((bg << 4) &0xf0) | (fg &0xf);
        val |=  clor << 8;

        fp->curp = (unsigned char*)(fp->buffer + fp->off);
	    *(unsigned short*)(fp->curp) = val &0xffff;

	    // Update offset
	    fp->off += 2;
	    if(fp->off > fp->fsize ) fp->fsize += 2;

    }else {
	    fp->curp = (unsigned char*)(fp->buffer + fp->off);
	    *(unsigned char*)(fp->curp) = c &0xff;

	    // Update offset
	    fp->off += 1;
	    if(fp->off > fp->fsize ) fp->fsize += 1;
	}
	
	return (c);

}


int stdgetc_r(FILE *fp)
{	
	int r = 0;

    if(fp->off2 >= fp->bsize) {
	
		fp->off2 = 0;
	}
	
	if(fp->flags == 2 ) { // stdin

		while(fp->off <= fp->off2 && fp->off >= fp->off2){} // wait
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

        fp->off2++;

        //stdputc_r (r, stdout);		

	}else if(fp->flags == 4)  { // stderr


		if(!fp->off) return EOF;

		if(fp->off2 >= fp->off ) return EOF;

		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

		fp->off2++;

	}else if(fp->flags == 3){ // stdout

		if(fp->off2 == fp->off ) return EOF;

		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

		fp->off2 += 2;
    }


	return (r);
}


int getkey() {

	FILE *fp = stdin;

    if(fp->off2 >= fp->bsize) {
	
		fp->off2 = 0;
	}	

	int r = 0;
	
	if(fp->flags == 2 ) { // stdin

		
		if(fp->off <= fp->off2 && fp->off >= fp->off2) return 0;
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

        fp->off2++;
		
		// if(r == EOF) r = 0;
	}


	return (r);
}


int getkeyw() {

	FILE *fp = stdin;	

	int r = 0;

    if(fp->off2 >= fp->bsize) {
	
		fp->off2 = 0;
	}
	
	if(fp->flags == 2 ) { // stdin

		
		while(fp->off <= fp->off2 && fp->off >= fp->off2){} // wait
		
		fp->curp = (unsigned char*)(fp->buffer + fp->off2);
		r = *(unsigned char*)(fp->curp) &0xff;

        fp->off2++;
		
		// if(r == EOF) r = 0;
	}


	return (r);
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


FILE *open_r (const char *path,const char *mode) 
{
	if(mode[0] == '\0') return NULL;

	FILE *fp = open_file(path,mode);

	return fp;
	
}

int flush_r(FILE *fp) {

	if(!fp) return -1;

	if(!fp || !(fp->flags&0x6) || fp->mode != 2 ) return -1;
	
	if(!(fp->flags&0x80)) return 0;
	
	int addr = fp->off/fp->bsize;
	
	if( !(fp->flags&0x40) ) file_write_block(fp, addr);
	
	// actualizar os dados da entra de directorio
	int r = file_update(fp);
	
	return r;
}

int close_r(FILE *fp)
{
	if(!fp) return -1;
	
	flush_r(fp);
	
	return file_close(fp);
}


int putc_r (int ch, FILE *fp) 
{

	if(!fp) return -1;

	if(fp->flags == 5) return 0; // pipe
	
	if(fp->flags == 2 || fp->flags == 3 || fp->flags == 4 ) { 
	
		return stdputc_r (ch, fp);
	}
	
	
	if(!(fp->flags&0x10) && fp->off <= fp->fsize && fp->fsize != 0 ) {
		// ler
		fp->flags |= 0x10;
		
		int addr = fp->off/fp->bsize;
		file_read_block(fp, addr);
	}else {
	
		fp->flags |= 0x10;
	}
	
	fp->level = fp->off%fp->bsize;
	
	fp->curp = (unsigned char*)(fp->buffer + fp->level);
	*(unsigned char*)(fp->curp) = ch &0xff;
	

	// update
	if(fp->level >= (fp->bsize-1)) {
		fp->flags &=~0x10;
		fp->level = 0;
		
		//flush
		flush_r(fp);
		// novo bloco.
        // TODO:
		
	}
	
	fp->off ++;
	if(fp->off > fp->fsize ) fp->fsize ++;
	
	fp->flags |= 0x80;
	
	return 0;

}

int getc_r (FILE *fp)
{	
	int r;
	
	if(!fp) return -1;
	
	if(fp->flags == 5) return 0; // pipe
	
	if(fp->flags == 2 || fp->flags == 3 || fp->flags == 4 ) { 
	
		r = stdgetc_r (fp);
		
		return r;
	}
	

	
	if(fp->off >= fp->fsize) {
	
		return EOF;
	}
	
	
	if(!(fp->flags&0x10) && fp->off <= fp->fsize && fp->fsize != 0 ) {
		// ler
		fp->flags |= 0x10;
		
		int addr = fp->off/fp->bsize;
		file_read_block(fp, addr);
	}


	fp->level = fp->off%fp->bsize;


	fp->curp = (unsigned char*)(fp->buffer + fp->level);
	r = *(unsigned char*)(fp->curp) &0xff;

	// update
	if(fp->level >= (fp->bsize-1)) {
		fp->flags &=~0x10;
		fp->level = 0;
		
	}
	
	fp->off++;
	
	return (r);
}

size_t read_r (void *buffer,size_t size, size_t count, FILE *fp)
{
	if(!fp) return (0);
	int c;
	size_t rc = 0;

	unsigned char *buf = (unsigned char*)buffer;

	for(int i=0;i < size*count;i++)
	{
		c = getc_r (fp);
			//feof(fp);
		if(c == EOF) return (rc/size);

		*buf ++ = c;
		rc++;
	}

	return (rc/size);
}


size_t write_r (const void *buffer, size_t size, size_t count, FILE *fp)
{
	if(!fp) return (0);
	int c;
	size_t rc = 0;

	unsigned char *buf = (unsigned char*)buffer;

	for(int i=0;i < size*count;i++)
	{
		c = *buf ++;
		putc_r (c, fp);
		rc++;
	}

	return (rc/size);
}

int seek_r (FILE *fp, long num_bytes, int origin )
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

long int tell_r(FILE *fp)
{
	if(!fp) return (0);

	return (fp->off);

}



void rewind_r(FILE *fp)
{
	if(!fp) return;
	
	fp->off = fp->off2 = 0;
	fp->level = 0;
	
	// define leitura
	fp->flags &=~0x10;

}
