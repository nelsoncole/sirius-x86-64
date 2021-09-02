#include <fs.h>
#include <storage.h>
#include <stdlib.h>
#include <string.h>
#include <data.h>
directory_entry_t fs_directory[128];

int filename_cmp(const char *s1, const char *s2)
{

	char s3[256];
	
	unsigned char *us1 = (unsigned char *)s1;
	unsigned char *us2 = (unsigned char *)s2;
	unsigned char *us3 = (unsigned char *)s3;
	int i;
	
	for(i=0; i < 96; i++) { 
	
		if(!*us2) {
			us3[i] = ' ';
		} else {
			us3[i] = *us2++;
		}
	}
	
	for(i=0;i < 96; i++) {
	
		if(*us1++ != *us3++) {
			return 1;
		}
	}
	
	return 0;
}

int read_super_block(super_block_t *f,part_t *p)
{
	// Primeiro sector de particao
	return( read_sector(p->dv_num,1, p->start,f) );
}

int read_directory_entry(directory_entry_t *d,super_block_t *f,part_t *p,int type, const char *filename)
{
	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (f->root_blk * f->sector_per_blk) + data_sector;
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	char *root = (char*)malloc(8192);
	memset(root,0,8192);
	
	int r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	// Pesquizar em todos os blocos
	int i;
	for(i=0; i < 64; i++) {
	
		r = filename_cmp((const char *)root + (i*128), filename);
	
		if(!r) break;
	}
	
	memcpy(d, root+(i*128), 128);
	
	free(root);

	return 0;	
}


FILE *open_file_r(directory_entry_t *d,super_block_t *f,part_t *p)
{

	FILE *fp = (FILE *)malloc(sizeof(FILE));
	memset(fp,0,sizeof(FILE));
	
	fp->bsize 	= (unsigned) BUFSIZ;
	fp->buffer 	= (unsigned char*)malloc(BUFSIZ);
	fp->fsize	= d->file_size;
	fp->byte_per_sector = f->byte_per_sector;
	fp->count	= f->sector_per_blk;
	
	fp->dv_num	= p->dv_num;
	
	// 1024*1024
	fp->blocks	= (unsigned long*)malloc(4096); // 4KiB
	
	unsigned int index = d->first_blk;
	// obter blocos
	unsigned int *BLK = (unsigned int*)malloc(8192); // 8KiB
	unsigned int *blocks;
	unsigned int start,flg_start = -1;
	int i,t,r,eof = 0;
	for(i=0;i<1024;i++) {
	
		fp->blocks[i] = (unsigned int)malloc(4096); // 4KiB
		blocks = (unsigned int*) fp->blocks[i];
		memset(blocks,0,4096);

		for(t=0;t<1024;t++) {
			blocks[t] = (index*f->sector_per_blk) + f->rsv + f->hidden;
			fp->num_of_blocks++;
			
			// verifique eof
			start = ((index * 4) / f->byte_per_sector) + f->rsv + f->hidden;
			
			if(flg_start != start) {
			
				r = read_sector(p->dv_num, 1, start,BLK);
				if(r) {
			
					// error
					free(BLK);
					file_close(fp);
					return NULL;
				}
				
				flg_start = start;
			}
		
			index = index % (f->byte_per_sector/4);
			
			index = BLK[index];
			
			if(index == EOF) {
			
				eof = -1;
				break;
			}	
		}
		
		if(eof == EOF) break;
	}
	
	free(BLK);
	return (fp);
}

FILE *open_file(const char *filename, const char *mode)
{

	super_block_t *f = (super_block_t *) malloc(1024);
	memset(f,0,1024);
	
	part_t p[1];
	memset(p,0,sizeof(part_t));
	
	
	// initialize partition
	p->start = 1;
	p->dv_num = dv_num;
	
	int r = read_super_block(f,p);
	
	if(r) {
	
		free(f);
		return 0;
	}

	r = read_directory_entry(fs_directory,f,p,0,filename);
	
	if(r) {
	
		free(f);
		return 0;
	}

	
	FILE *fp = open_file_r(fs_directory,f,p);
	
	free(f);
	return fp;
}

int file_close(FILE *fp)
{
	int i;
	free(fp->buffer);
	
	for(i=0;i < 1024;i++){

		if(!fp->blocks[i]) break;
		free((void*)fp->blocks[i]);
	}
	
	free(fp->blocks);
	free(fp);
	
	fp = NULL;
	return 0;
}


int file_read_block(FILE *fp,int addr)
{

	if(addr >= fp->num_of_blocks || (!fp) ) return -1;

	unsigned int *blocks = (unsigned int*) fp->blocks[addr/1024];
	unsigned int start = blocks[addr%1024];

	return( read_sector(fp->dv_num, fp->count, start,fp->buffer) );
}


