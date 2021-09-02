#include <stdio.h>



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

extern FILE *open_r (const char *path,const char *mode);
FILE *fopen (const char *path,const char *mode) 
{
	if(mode[0] == '\0') return NULL;


	if(mode[0] == 'w') remove (path);

	FILE *fp = open_r ( path, mode );

	return fp;
	
}
