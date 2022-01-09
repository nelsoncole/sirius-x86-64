#ifndef __ETHER_H__
#define __ETHER_H__

typedef struct __ether_header {

	unsigned char   dst[SIZE_OF_MAC];
	unsigned char   src[SIZE_OF_MAC];
	unsigned short  type;

} __attribute__((packed)) ether_header_t;


#endif
