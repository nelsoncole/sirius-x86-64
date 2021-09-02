
#ifndef __BMP_H
#define __BMP_H


typedef struct _BMP_FILE {
	unsigned short type;
	unsigned int 	size;
	unsigned short	rsv1;
	unsigned short	rsv2;
	unsigned int	offset_bits;
}__attribute__((packed)) BMP_FILE;

typedef struct _BMP_INFO {
	unsigned int 	size;
	unsigned int 	width;
	unsigned int	height;
	unsigned short	planes;
	unsigned short	count;
	unsigned int	compress;
	unsigned int	size_imag;
	unsigned int	hres;
	unsigned int	vres;
	unsigned int	clr_used;
	unsigned int	clr_impor;
	
	
}__attribute__((packed)) BMP_INFO;

typedef struct _BMP_TABLE_COLOR {
	unsigned char 	blue;
	unsigned char 	green;
	unsigned char	red;
	unsigned char	rsv;
	
}__attribute__((packed)) BMP_TABLE_COLOR;

typedef struct _BMP {
	BMP_FILE bformat;
	BMP_INFO binfo;
}__attribute__((packed)) BMP;



#endif
