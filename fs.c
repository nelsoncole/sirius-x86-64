#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _super_block {

	char sig[8];				// 00 - 7
	char name[12];				// 08 - 19
	unsigned int hidden;			// 20 - 23
	unsigned int reserved;			// 24 - 27
	unsigned int byte_per_sector;		// 28 - 31
	unsigned int sector_per_blk;		// 32 - 35
	unsigned int num_of_blk;		// 36 - 39
	unsigned int size_of_blk;		// 40 - 43
	unsigned int root_blk;			// 44 - 47
	unsigned int total_num_of_sector;	// 48 - 51
	

}__attribute__ ((packed)) super_block;

typedef struct _directory_entry {

	char 		filename[96];
	unsigned char 	rsv;
	unsigned char 	busy;
	unsigned char 	attr;
	unsigned short	time;
	unsigned short	time2;
	unsigned short	date;
	unsigned short	date2;
	unsigned int 	blk;
	unsigned int 	filesize;
	unsigned int 	entry;
	char 		rsv2[9];
}__attribute__ ((packed)) directory_entry;


unsigned int disk_size = 0;

FILE *fd1;

unsigned char data[512];

char __filename[128];
const char *print_filename(const char *filename){

	
	memcpy(__filename,filename,96);
	
	__filename[96] = '\0';
	
	for(int i=95; i >= 0; i--){
	
		if(__filename[i] == ' ') __filename[i] = '\0';
		else {
			break;
		}
	
	}
	
	return __filename;
}


void read_super_block(FILE *fd,super_block *fs)
{

	// inicio de particao + 1;
	int off = 1*512;
	
	fseek (fd, off, SEEK_SET );
	fread(fs,1,sizeof(super_block),fd);	
	rewind(fd);
}

unsigned int size_of_blk(super_block *fs)
{
	
	unsigned int n = (((fs->total_num_of_sector - \
	fs->reserved)/fs->sector_per_blk)*4)/(fs->byte_per_sector * fs->sector_per_blk);
	
	if( ( (((fs->total_num_of_sector - \
	fs->reserved)/fs->sector_per_blk)*4)%(fs->byte_per_sector * fs->sector_per_blk) ) ) n += 1;
	return (n);
}

unsigned int num_of_blk(super_block *fs)
{
	return( (fs->total_num_of_sector - fs->reserved)/fs->sector_per_blk );
}


void format_blk(FILE *fd,super_block *fs)
{

	unsigned int data = 0;
	
	int i;
	
	fseek (fd, fs->reserved*fs->byte_per_sector, SEEK_SET );

	for(i=0; i < fs->num_of_blk;i++){
	 
		if(i < fs->size_of_blk)data =0x8fffffff;
		else data = 0;
		
		fwrite(&data,1,4,fd);
	}

	fflush(fd);
	
	rewind(fd);
}

void clean_blk_enter(unsigned int blk,FILE *fd,super_block *fs)
{
	unsigned int data = 0;
	unsigned int data_sector =  fs->reserved;
      	unsigned int first_sector = data_sector + (blk * fs->sector_per_blk);
      	unsigned int off = first_sector * fs->byte_per_sector;
      	
      	int i, n = (fs->byte_per_sector * fs->sector_per_blk)/4;
      	
	fseek (fd, off, SEEK_SET );
	for(i=0 ;i < n;i++) fwrite(&data,1,4,fd);

	fflush(fd);
	rewind(fd);
}

void root_dir(FILE *fd,super_block *fs)
{
	unsigned int data = -1;
	int i;
	
	fs->root_blk = fs->size_of_blk;
	
	int off = (fs->reserved*fs->byte_per_sector) + (fs->root_blk*4);
	
	fseek (fd, off, SEEK_SET );
	
	fwrite(&data,1,4,fd);
	
	fflush(fd);
	
	
	//clear directory
	off = (fs->reserved*fs->byte_per_sector) + (fs->sector_per_blk*fs->byte_per_sector*fs->root_blk);
	
	fseek (fd, off, SEEK_SET );
	
	data = 0;
	for(i=0; i < (fs->sector_per_blk*fs->byte_per_sector/4);i++){
		
		fwrite(&data,1,4,fd);
	}
	
	fflush(fd);
	rewind(fd);
	
	
	clean_blk_enter(fs->root_blk,fd,fs);
}

unsigned int search_blk_null(FILE *fd,super_block *fs)
{
	unsigned int blk;
	unsigned int index = 0;
	unsigned int off = fs->reserved * fs->byte_per_sector;
	
	fseek (fd, off, SEEK_SET );
	
	for(index = 0;index < fs->num_of_blk;index++) {
	
		fread(&blk,1,4,fd);
		
		if(!blk){
		
			// gravar o index
			blk = -1;
			fseek (fd, off, SEEK_SET );
			fwrite(&blk,1,4,fd);
			fflush(fd);
		
			rewind(fd);
			return (index);
		}
		off += 4;
		
	}
	
	rewind(fd);
	return (-1);

}

int write_blk(unsigned int new_blk,unsigned int cur_blk,FILE *fd,super_block *fs)
{
	int r;
	unsigned int blk = new_blk;
	unsigned int off = (fs->reserved * fs->byte_per_sector) + (4*cur_blk);
	
	
	
	fseek (fd, off, SEEK_SET );
	r = fwrite(&blk,1,4,fd);
	if(r != 4) return -1;
	
	fflush(fd);
	rewind(fd);

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

int create_file(const char *path,FILE *fd,super_block *fs,directory_entry *directory, int attr) {
	int index;
	unsigned int off;
	
	directory_entry *dir = directory;
	directory_entry dir2[1];
	
	memset(dir,0,128);
	
	
	FILE *f;
	// Testar se arquivo existe
	if(attr == 0x20) {
		if((f=fopen(path,"r"))==NULL) {
			printf("Erro '%s': Ficheiro ou pasta inexistente",path);
			return -1; 
		}else fclose(f);
	}
	
	strcpy(dir->filename,get_file_name(path));
	
	// Limpar os caracters nulo
	off = 0;
	for(index = 0; index < 96; index++) { 
	
		if(dir->filename[index] == 0 || off == 1) {
		
			dir->filename[index] = ' ';
			off = 0;
		}
	}
	
	dir->filesize = 0;
      	
      	
      	unsigned int data_sector =  fs->reserved;
      	unsigned int root_sector = data_sector + (fs->root_blk * fs->sector_per_blk);
	
	
	
	// localizar blk disponivel
	dir->blk = search_blk_null(fd,fs);
	// verificar erro
	if(dir->blk == -1) {
		printf("Error ao criar o arquivo \"%s\"",path);
	 	return 1;
	}


	//numero de entrada, aqui tem que pesquisar
	for(index = 0;index < 64 ;index++) {
		off = (root_sector * fs->byte_per_sector) + (128*index);
		fseek (fd, off, SEEK_SET );
		fread(dir2,1,128,fd);
		//dir->busy == 0, entrada livre
		if(!dir2->busy) break;
	}
	
	
	
	if(index >= 64 ) {
		printf("Error ao criar o arquivo \"%s\", atingio o limite de arquivos na entrada de directorio",path);
		return -1;
	}
	
	
	// gravar directory
	dir->entry = index;
	dir->busy = -1;
	dir->attr = attr; //0x20; // archive
	
	off = (root_sector * fs->byte_per_sector) + (128*dir->entry);
	fseek (fd, off, SEEK_SET );
	fwrite(dir,1,128,fd);
	fflush(fd);
	rewind(fd);
	
	return 0;
}

void copy(FILE *fd,super_block *fs,directory_entry *directory, const char *path) {

	FILE *f;
	directory_entry *dir = directory;
	
	if((f=fopen(path,"r+b"))==NULL) {
		printf("Erro ao copiar  o arquivo: \"%s\"",path);
		return; 
	}
	
	
	unsigned int data_sector =  fs->reserved;
      	unsigned int first_sector = data_sector + (directory->blk * fs->sector_per_blk);
      	unsigned int root_sector = data_sector + (fs->root_blk * fs->sector_per_blk);
      	unsigned int off;
      	unsigned int size_blk = fs->sector_per_blk * fs->byte_per_sector;
      	
      	
      	// gravar directory, tamanho em bytes
      	fseek (f, 0, SEEK_END );
	dir->filesize = ftell(f);
      	rewind(f);
      	
	off = (root_sector * fs->byte_per_sector) + (128*dir->entry);
	fseek (fd, off, SEEK_SET );
	fwrite(dir,1,128,fd);
	fflush(fd);
	
	
	off = first_sector * fs->byte_per_sector;
	
	int i,n = 1;
	unsigned int new_blk;
	unsigned int cur_blk = dir->blk;
	
	fseek (fd, off, SEEK_SET );
	
	for(i=0;i<(directory->filesize);i++){
	
		if(i == (size_blk*n)) { // new bloco
		
			// obter novo bloco
			new_blk = search_blk_null(fd,fs);
			// valor do ultimo blk e ecrever nele o valor do novo bloco
			write_blk(new_blk,cur_blk,fd,fs);
		
			cur_blk = new_blk;
			n++; 
			
			// new offset
			first_sector = data_sector + (new_blk* fs->sector_per_blk);
			off = first_sector * fs->byte_per_sector;
			fseek (fd, off, SEEK_SET );
			
			
		}
		
		fputc(getc(f),fd);
		
	}
	
	fflush(fd);
	rewind(fd);
	
	fclose(f);
}
 
int main(int argc, char **argv) {

	int r,d,a,flg;
	FILE *fd_boot;
	

	super_block fs;
	directory_entry directory[1];
	
	memset(directory,0,sizeof(directory_entry));
	memset((char*)&fs,0,sizeof(super_block));
	
	if(argc < 3) {
	
		printf("Argumento invalido\n"); 
		return 0;
	}
	
	if( !strcmp("-f",argv[1]) ) {
		d = 2;
		if(argc > 3 && argc < 6) {
		
			if( !strcmp("-g",argv[3]) ) {
			
				a = 4;
				flg = 2;
			
			} else {
				printf("Argumento invalido\n");
				return 0;
			}
		
		} else if(argc == 3) { 
			flg = 1;
		} else {
			printf("Argumento invalido\n");
			return 0;
		}
		
	}else if(!strcmp("-g",argv[1])) {
	
		a = 2;
		
		if(argc == 4) {
			flg = 3;
			d = 3;
		} else if(argc > 3 && argc < 6) {
		
			if( !strcmp("-f",argv[3]) ) {
			
				d = 4;
				
				flg = 2;
			
			} else {
				printf("Argumento invalido\n");
				return 0;
			}
		
		} else {
			printf("Argumento invalido\n");
			return 0;
		}
	}else if(!strcmp("-p",argv[1])) {
		a = 2;
		
		if(argc == 4) {
			d = 3;
			flg = 4;
		} else {
			printf("Argumento invalido\n");
			return 0;
		}
		 
	}else {
	
		printf("Argumento invalido\n");
		return 0;
	}

      	memset(data,0,512);
      	
      	strcpy(fs.sig,"KINGUIO_");
      	
      	if((fd1=fopen(argv[d],"r+w"))==NULL) {
		printf("Erro ao abrir o disco \"%s\"",argv[d]);
		exit(1); 
	}
      	
      	
	if(flg == 3) {
		read_super_block(fd1,&fs);
	 	goto __gravar;
	 	
	}else if(flg == 4){
		read_super_block(fd1,&fs);
		goto __folder;
		
	} else if(flg > 3 || flg == 0) return 0;
	
	
	fs.reserved = 2;
	fs.byte_per_sector = 512;
	fs.sector_per_blk = 16;
	
	fseek (fd1, 0, SEEK_END );
	disk_size = ftell(fd1);
      	rewind(fd1);
      	
      	fs.total_num_of_sector = disk_size /fs.byte_per_sector;
      	fs.num_of_blk = num_of_blk(&fs);
      	fs.size_of_blk = size_of_blk(&fs);
	
// -f
//__format;
      	
      	// formatar a tabela de blocos
      	format_blk(fd1,&fs);
      	fs.root_blk = 0;
      	//Criar o directorio raiz
      	root_dir(fd1,&fs);
	
	// Gravar o bootrecord
	if((fd_boot=fopen("bin/stage0.bin","r+b"))==NULL){
		printf("Erro ao abrir o arquivo \"bootrecord\"");
		exit(1); 
	}
	
	r = fread(data,1,fs.byte_per_sector,fd_boot);
	
	if(r != fs.byte_per_sector) {
		printf("Erro ao copiar o sector de boot");
		exit(1);
	}
	
		
	r = fwrite(data,1,fs.byte_per_sector,fd1);
	if(r != fs.byte_per_sector) {
		printf("Erro ao ecrever o sector de boot");
		exit(1);
	}
	
	//Gravar o super bloco
	r = fwrite(&fs,1,sizeof(super_block),fd1);
	if(r != sizeof(super_block)) {
		printf("Erro ao gravar o super bloco");
		exit(1);
	}
	
	fclose(fd_boot);
	fflush(fd1);
	rewind(fd1);
	
	if(flg == 1) goto __end;

// -g
__gravar:
	
	// gravar arquivo
	r = create_file(argv[a],fd1,&fs,directory, 0x20);
	if(!r) {
		copy(fd1,&fs,directory,argv[a]);
		printf("Copiado: %s, %d bytes, blk %d, dir entry %d\n",
		print_filename(directory->filename), directory->filesize,directory->blk,directory->entry);
	}
	goto __f;
	
__folder:
	create_file(argv[a],fd1,&fs,directory, 0x40);
__f:	
	
__end:
	fclose(fd1);
	printf("done\n");
	exit(0);
	
	return 0;
}
