#include <file.h>
#include <stdlib.h>
#include <string.h>

#include <path.h>

unsigned int _BLK_[512];


int read_sector(int dev,unsigned count,unsigned long long addr,void *buffer)
{	int ret = -1;
	__asm__ __volatile__("int $0x72;":"=a"(ret):"d"(6),"c"(count),"S"(addr),"D"(buffer));
	
	return ret;
}
int write_sector(int dev,unsigned count,unsigned long long addr,void *buffer)
{	int ret = -1;
	__asm__ __volatile__("int $0x72;":"=a"(ret):"d"(7),"c"(count),"S"(addr),"D"(buffer));
	
	return ret;
}

directory_entry_t *fs_directory;
int assert_fn(char *fn){
	
	for(int i=95; i >= 0; i--){
	
		if(fn[i] == ' ') fn[i] = '\0';
		else {
			break;
		}
	
	}
	
	return 0;
}

char *get_file_name(const char *path)
{
	char *s = (char*) path;
	char *r = s;

	for(;*s != 0;s++) {
	
		if(*s == '/') {
			r = s + 1;
		}	
	}

	return r;
}

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

unsigned int search_blk_null(part_t *p, super_block_t *f)
{

	unsigned int start = f->rsv + f->hidden;
			
	int r = read_sector(p->dv_num, 4, start, _BLK_);
	if(r) return (-1);
	
	
	for(unsigned int i = 0; i < f->num_of_blk; i ++) {
				
		if(!_BLK_[i%512] ){
		
			// gravar o index
			_BLK_[i%512] = -1;
			
			r = write_sector(p->dv_num, 4, start, _BLK_);
			if(r) return (-1);
			
			return (i);
		}
		
		if(i%512 == 512-1 ) {
		
			start += 4;
			r = read_sector(p->dv_num, 4, start, _BLK_);
			if(r) return (-1);
		}
		
	}
	
	return (-1);

}

// esta funcao copia bloco para bloco
int write_blk(unsigned int new_blk,unsigned int cur_blk, part_t *p, super_block_t *f)
{

	return 0;
}

// esta funcao limpa o bloco
void clean_blk_enter(unsigned int blk, part_t *p, super_block_t *f)
{
	unsigned int data_sector =  f->rsv + f->hidden;
      	unsigned int first_sector = data_sector + (blk * f->sector_per_blk);
      	
      	memset(_BLK_, 0, 512*sizeof(unsigned int));
      	int r;
	for(int i=0 ;i < f->sector_per_blk; i ++) {
		r = write_sector(p->dv_num, 1, first_sector + i, _BLK_);
		if(r) return;
	}

}


int read_super_block(super_block_t *f, part_t *p)
{
	// Primeiro sector de particao
	return( read_sector(p->dv_num, 1, p->start,f) );
}

unsigned int read_directory_blk(const char *path, part_t *p, super_block_t *f)
{
	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (f->root_blk * f->sector_per_blk) + data_sector;
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	char *root = (char*)malloc(8192);
	char *tpath = (char *)path;
	
	int count = path_count(tpath);
	unsigned int r = f->root_blk;
	char *filename;
	directory_entry_t *dir;
	
	
	//memset(root,0,8192);
	
	while( count > 0) {
	
		
		filename = tpath;
		while( *tpath != 0) {
			if(*tpath == '/') {
				*tpath = '\0';
				tpath ++;
				break;
			}
			
			tpath ++;
		}
		
		r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
		if(r) { 
			free(root);
			return 0;
		}
	
		// Pesquizar em todos os blocos
		int i;
		for(i=0; i < 64; i++) {
	
			dir = (directory_entry_t*) ((unsigned long)root + (i*128));
			if(dir->busy != 0 && (dir->attr&0x40) != 0) {
				r = filename_cmp( dir->file_name, filename);
			}else r = -1;
		
			if(!r) break;
		}
	
	
		if(!r && i < 64) {
			r = dir->first_blk;
		}else { 
			r = 0; // err
			break;
		}
		
		first_sector = (r * f->sector_per_blk) + data_sector;
		
		count --;
	}
	
	free(root);
	return r;

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
	
	if(r) { 
		free(root); 
		return -1;
	}
	
	
	// Pesquizar em todos os blocos
	int i;
	for(i=0; i < 64; i++) {
	
		directory_entry_t *dir = (directory_entry_t*) ((unsigned long)root + (i*128));
		if(dir->busy != 0) {
			r = filename_cmp( dir->file_name, filename);
		}else r = - 1;
		
		if(!r) break;
	}
	
	
	if(!r && i < 64) {
		memcpy(d, root+(i*128), 128);
	}else r = -1; // err
	
	free(root);

	return r;	
}


int update_directory_entry( super_block_t *f, part_t *p, FILE *fp)
{

	if(fp->dir_entry < 1) return -1;
	
	if(fp->dir_entry > 64) {
		printf("Entrada maior que o limite\n");
		return -1;
	}
	
	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (fp->dir_cluster * f->sector_per_blk) + data_sector;
	
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	directory_entry_t *root = (directory_entry_t *)malloc(8192);
	memset(root,0,8192);
	
	int r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	if(r){
	
		free(root);
		return r;
	}
	
	directory_entry_t *dir = (directory_entry_t *) (unsigned long)(root + fp->dir_entry);
	
	memset(dir->file_name, 0, 96);
	strcpy(dir->file_name,get_file_name(fp->fname));
	
	// Limpar os caracters nulo
	for(int i = 0; i < 96; i ++) { 
	
		if(dir->file_name[i] == 0) {
		
			dir->file_name[i] = ' ';
		}
	}
	
	dir->rsv = 0;
	dir->file_size = fp->fsize;
	
	r = write_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	free(root);
	
	return r;
}

FILE *open_file_r(directory_entry_t *d,super_block_t *f,part_t *p)
{

	FILE *fp = (FILE *)malloc(sizeof(FILE));
	memset(fp,0,sizeof(FILE));
	
	fp->bsize 	= (unsigned) (f->byte_per_sector*f->sector_per_blk);
	fp->buffer 	= (unsigned char*) malloc(BUFSIZ);
	fp->fsize	= d->file_size;
	fp->byte_per_sector = f->byte_per_sector;
	fp->count	= f->sector_per_blk;
	
	fp->dv_num	= p->dv_num;
	
	
	
	// 1024*1024
	fp->blocks	= (unsigned long*)malloc(1024*sizeof(unsigned long)); // 4KiB
	memset(fp->blocks,0,1024*sizeof(unsigned long));
	
	unsigned int index = d->first_blk;
	
	// obter blocos
	unsigned int *BLK = (unsigned int*)malloc(8192); // 8KiB
	unsigned int *blocks;
	unsigned int start,flg_start = -1;
	int i,t,r,eof = 0;
	
	for(i=0;i<1024;i++) {
	
		fp->blocks[i] = (unsigned long)malloc(4096); // 4KiB
		blocks = (unsigned int*) (unsigned long)fp->blocks[i];
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


	char *path = (char*)malloc(0x1000 + 256);
	char *tpath = path;
	char *_filename = path + 0x1000 ;
	
	getpathname(tpath, filename);
	getfilename(_filename, tpath);


	fs_directory = (directory_entry_t *) malloc(sizeof(directory_entry_t));
	memset(fs_directory,0,sizeof(directory_entry_t));

	super_block_t *f = (super_block_t *) malloc(1024);
	memset(f,0,1024);
	
	part_t _p[1];
	part_t *p = (part_t *)(unsigned long)&_p;
	memset(p,0,sizeof(part_t));

	int flg = 0;
	char m[256] ={0,0,0,0};
	strcpy(m,mode);
	
	if(m[0] == 'w' || m[0] == 'a') flg = 0x1; // criar arquivo caso nao existir
	
	// initialize partition
	p->start = 1;
	p->dv_num = 0;
	
	int r = read_super_block(f,p);
	if(r) {
		free(fs_directory);
		free(path);
		free(f);
		return 0;
	}
	
	
	
	tpath += 3;
	
	unsigned int blk = read_directory_blk((const char *)tpath, p,f);
	if(! blk ) {
		free(fs_directory);
		free(path);
		free(f);
		return 0;
	}
	
	f->root_blk  = blk;

	r = read_directory_entry(fs_directory,f,p,0,_filename);
	
	if(r == -1) {
	
		if(flg) {
	
			create_file(fs_directory,f,p, filename, 0x20);
			r = read_directory_entry(fs_directory,f,p,0,filename);
		}
		
		if(r != 0) { 
			free(fs_directory);
			free(path);
			free(f);
			return 0;
		}
	}else if(r != 0){
		free(path);
		free(fs_directory);
		free(f);
		return 0;
	}

	
	FILE *fp = open_file_r(fs_directory,f,p);
	if(fp) {
	
		memcpy(fp->fname, fs_directory->file_name , 96);
		assert_fn(fp->fname);
	
		if(m[0] == 'w' || m[1] == '+' || m[0] == 'a') fp->mode = 0x2;
		else fp->mode = 0x1;
		
		fp->flags	= 0x6;
		
		fp->dir_cluster = f->root_blk;
		fp->dir_entry 	= fs_directory->dir_entry;
		
		if(m[0] == 'a') { 
			fp->off = fs_directory->file_size;
			
		}
	}
	
	free(path);
	free(fs_directory);
	free(f);
	return fp;
}

int file_update(FILE *fp)
{
	super_block_t *f = (super_block_t *) malloc(1024);
	memset(f,0,1024);
	
	part_t _p[1];
	part_t *p = (part_t *)(unsigned long)&_p;
	memset(p,0,sizeof(part_t));
	
	
	// initialize partition
	p->start = 1;
	p->dv_num = 0;
	
	int r = read_super_block(f, p);
	
	if(r) {
		free(f);
		return -1;
	}
	
	r = update_directory_entry(f, p, fp);
	free(f);
	return r;
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

int file_write_block(FILE *fp,int addr)
{

	if(addr >= fp->num_of_blocks || (!fp) ) return -1;

	unsigned int *blocks = (unsigned int*) fp->blocks[addr/1024];
	unsigned int start = blocks[addr%1024];

	return( write_sector(fp->dv_num, fp->count, start,fp->buffer) );
}


int open_dir(const char *pathname, void *buf, int count)
{
	
	char *path = (char*)malloc(0x1000 + 256);
	char *tpath = path;
	char *filename = path + 0x1000 ;
	
	getpathname(tpath, pathname);
	getfilename(filename, tpath);
	
	
	super_block_t *f = (super_block_t *) malloc(1024);
	memset(f,0,1024);
	
	part_t _p[1];
	part_t *p = (part_t *)(unsigned long)&_p;
	memset(p,0,sizeof(part_t));
	
	
	// initialize partition
	p->start = 1;
	p->dv_num = 0;
	
	int r = read_super_block(f,p);
	
	if(r) {
		free(fs_directory);
		free(path);
		free(f);
		return -1;
	}
	
	tpath += 3;
	
	unsigned int blk = read_directory_blk((const char *)tpath, p,f);
	if(! blk ) {
		free(fs_directory);
		free(path);
		free(f);
		return -1;
	}
	
	f->root_blk  = blk;

	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (f->root_blk * f->sector_per_blk) + data_sector;
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	char *root = (char*)malloc(8192);
	memset(root,0,8192);
	
	r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	// Pesquizar em todos os blocos
	int v = 0;
	for(int i=0; i < 64; i++) {
	
		directory_entry_t *d = (directory_entry_t *) ((unsigned long)root + (i*128)); 
	
		if(i >= count) break;
		
		if(!d->busy) continue;
		
		memcpy((char *)buf + (v*128), d, 128);
		
		assert_fn((char *)buf + (v*128));
		
		v ++;
		
	}
	
	free(root);
	free(path);
	free(f);
	return v;	
}


int create_file(directory_entry_t *d,super_block_t *f,part_t *p, const char *path, int attr) {
	int i, r;

	directory_entry_t *dir = d;
	memset(dir,0,128);

	// Testar se arquivo existe
		
	strcpy(dir->file_name,get_file_name(path));
	
	// Limpar os caracters nulo
	for(i = 0; i < 96; i ++) { 
	
		if(dir->file_name[i] == 0) {
			dir->file_name[i] = ' ';
		}
	}
	
	
      	
      	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (f->root_blk * f->sector_per_blk) + data_sector;
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	char *root = (char*)malloc(8192);
	memset(root,0,8192);
	
	r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	if(r){
		free(root);
		return r;
	}
	
	// Pesquizar uma entrada livre em todos bloco
	directory_entry_t *tmp;
	for(i=0; i < 64; i++) {
	
		tmp = (directory_entry_t*) (unsigned long)(root + (i*128));
		if(!tmp->busy) break;
	}
	
	if(!tmp->busy && i < 64){ 
		// gravar directory
		dir->dir_entry = i;
		dir->busy = -1;
		dir->attr = attr; //0x20; // archive
		// localizar blk disponivel
		dir->first_blk = search_blk_null(p,f);
		// verificar erro
		if(dir->first_blk == -1) {
			printf("Erro ao criar o arquivo \"%s\"\n",path);
			
			free(root);
	 		return 1;
		}
		
		
		// clear block
		if(attr&0x40) clean_blk_enter(dir->first_blk, p, f);
		
		memcpy(tmp, dir, 128);
		r = write_sector(p->dv_num, f->sector_per_blk, first_sector,root);

	}else{
	

		free(root);
		printf("Error ao criar o arquivo \"%s\", atingio o limite de arquivos na entrada de directorio\n",path);
		return -1;
	}
	
	
	free(root);
	
	return 0;
}


int remove_blk(unsigned int first_blk, super_block_t *f, part_t *p )
{
	unsigned int *BLK = (unsigned int*)malloc(8192); // 8KiB
	
	int r, w = 0;
	
	unsigned int index = first_blk;
	unsigned int flg_start = -1;
	unsigned int start = ((index * 4) / f->byte_per_sector) + f->rsv + f->hidden;
	unsigned int wstart = start;
	
	for(;;) {

		if (start != flg_start ) {
		
			// write
			if(w != 0) {
		
				r = write_sector(p->dv_num, 1, wstart,BLK);
				if(r) {
		
					// error
					free(BLK);
					return -1;
				}
			}
	
	
			r = read_sector(p->dv_num, 1, start,BLK);
			if(r) {
			
				// error
				free(BLK);
				return -1;
			}
					
			flg_start = start;
			wstart = start;
			w = 1;
		}
		
		index = index % (f->byte_per_sector/4);
		
		unsigned int index_tmp = index;	
		index = BLK[index];
		BLK[index_tmp] = 0; // apagar bloco
			
		if(index == EOF) {
			r = write_sector(p->dv_num, 1, wstart, BLK);
			break;
		}	
	
		start = ((index * 4) / f->byte_per_sector) + f->rsv + f->hidden;
	
	}
	free(BLK);
	return r;
}

int remove_file(const char *path) {

	int r;
	
	char *p_path = (char*)malloc(0x1000 + 256);
	char *tpath = p_path;
	char *filename = p_path + 0x1000 ;
	
	getpathname(tpath, path);
	getfilename(filename, tpath);


	directory_entry_t *dir;
	
	super_block_t *f = (super_block_t *) malloc(1024);
	memset(f,0,1024);
	
	part_t _p[1];
	part_t *p = (part_t *)(unsigned long)&_p;
	memset(p,0,sizeof(part_t));
	
	// initialize partition
	p->start = 1;
	p->dv_num = 0;
	
	r = read_super_block(f,p);
	
	if(r) {
		free(p_path);
		free(f);
		return 0;
	}
	
	
	
	tpath += 3;
	
	unsigned int blk = read_directory_blk((const char *)tpath, p,f);
	if(! blk ) {

		free(p_path);
		free(f);
		return -1;
	}
	
	f->root_blk  = blk;
	
      	
      	// calcule o data_sector
	unsigned int data_sector = f->rsv + f->hidden;
	
	// calcule o first_sector
	unsigned int first_sector = (f->root_blk * f->sector_per_blk) + data_sector;
	
	// ler o primeiro bloco do directorio raiz 8KiB em geral.
	char *root = (char*)malloc(8192);
	memset(root,0,8192);
	
	r = read_sector(p->dv_num, f->sector_per_blk, first_sector,root);
	
	if(r){
		free(p_path);
		free(f);
		free(root);
		return r;
	}
	
	// Pesquizar em todos os blocos
	for(int i=0; i < 64; i++) {
		r = - 1;
		dir = (directory_entry_t*) ((unsigned long)root + (i*128));
		
		if (dir->busy != 0)
			r = filename_cmp(dir->file_name, filename);
	
		if(!r) { 
			// arquivo encontrada
			// apagar busy
			dir->busy = 0;
			
			// atualizar o root
			r = write_sector(p->dv_num, f->sector_per_blk, first_sector,root);
			if(r) break;
			
			break;
		}
		
	}
	
	
	if(r != 0) {
		free(p_path);
		free(f);
		free(root);
		return r;
		
	}
	
	
	// rmover blk
	r = remove_blk( dir->first_blk , f, p );
	
	free(p_path);
	free(f);
	free(root);
	
	return r;
}
