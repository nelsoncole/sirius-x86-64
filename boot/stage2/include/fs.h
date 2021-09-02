#ifndef __FS_H
#define __FS_H

#include <stdio.h>

typedef struct _part {
	unsigned int dv_num;
	unsigned int bytes_per_sector;
	unsigned int num_of_sectors;
	unsigned int start;
	unsigned int end;
	unsigned int filesystem_type;
	unsigned int id;
}__attribute__ ((packed)) part_t;


typedef struct _super_block {

	char 		sig[8];		// 00 - 7
	char 		name[12];		// 08 - 19
	unsigned int 	hidden;		// 20 - 23
	unsigned int 	rsv;			// 24 - 27
	unsigned int 	byte_per_sector;	// 28 - 31
	unsigned int 	sector_per_blk;	// 32 - 35
	unsigned int 	num_of_blk;		// 36 - 39
	unsigned int 	size_of_fat;		// 40 - 43
	unsigned int 	root_blk;		// 44 - 47
	unsigned int 	num_of_sectors;	// 48 - 51
	

}__attribute__ ((packed)) super_block_t;

typedef struct _directory_entry {

	char 		file_name[96];
	unsigned char 	rsv;
	unsigned char 	busy;
	unsigned char 	attr;
	unsigned short	time;
	unsigned short	time2;
	unsigned short	date;
	unsigned short	date2;
	unsigned int 	first_blk;
	unsigned int 	file_size;
	unsigned int 	dir_entry;
	char 		rsv2[9];
}__attribute__ ((packed)) directory_entry_t;

int file_read_block(FILE *fp,int addr);
FILE *open_file(const char *filename, const char *mode);
int file_close(FILE *fp);


#endif
