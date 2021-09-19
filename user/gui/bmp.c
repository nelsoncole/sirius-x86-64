#include <window.h>
#include <bmp.h>

#include <stdio.h>

#define Z00X 1
#define Z00Y 1
int BitMAP( void *Data, int X, int Y, int bg, WINDOW *w)
{
	BMP *bmp = (BMP*) Data;
	unsigned int *paleta = (unsigned int*)(((unsigned long)bmp) + 40 + 14);
	unsigned char *data_area = (unsigned char*) (((unsigned long)bmp) + bmp->bformat.offset_bits);
	unsigned int color;
	int x,y,i = 0;



	if(bmp->bformat.type != 0x4D42){
		printf("BitMAP error\n");
		return -1;

	}

	for(y = bmp->binfo.height; y > 0; y--) {
	    for(x = 0; x < bmp->binfo.width;  x++) {
	
		    // <= 8-Bit
		    if(bmp->binfo.count <= 8) {
		        if(bmp->binfo.count == 4) {
		            printf("Not suport BitMAP 4-bit");
		            return 2;
		
		        }else {
		            color = *(unsigned int*)((paleta) + *(unsigned char*/*count*/)(((unsigned long)data_area) + i++)) &0xffffff;
		        }
		    // > 8-Bit
		    }else if(bmp->binfo.count > 8){

		        if(bmp->binfo.count == 24){
			        color =  (*(unsigned int*)((data_area) + (3*i++))) &0xffffff;

		        }else if(bmp->binfo.count == 32){
                    color =  (*(unsigned int*)((data_area) + (4*i++))) &0xffffffff;

		        }else {
			        printf("Not suport BitMAP > 8-bit");
			        return 2;
		        }
		    }

		    // Aqui por onde ampliamos o pixel
		    for(int l =0; l < Z00Y; l++) 
		    {
			    for(int z =0; z < Z00X; z++) 
			    {
				    if(color != 0)
					    put_pixel(X + x*Z00X+z, Y + y*Z00Y+l, w->width, color, &w->start);
				    else
					    put_pixel(X + x*Z00X+z, Y + y*Z00Y+l, w->width, bg, &w->start);
			
			    }
		    }
		}
    }


	return 0;
}


int BitMAP2( void *Data, int X, int Y, int fg, int bg, WINDOW *w)
{
	BMP *bmp = (BMP*) Data;
	unsigned int *paleta = (unsigned int*)(((unsigned long)bmp) + 40 + 14);
	unsigned char *data_area = (unsigned char*) (((unsigned long)bmp) + bmp->bformat.offset_bits);
	unsigned int color;
	int x,y,i = 0;



	if(bmp->bformat.type != 0x4D42){
		printf("BitMAP error\n");
		return -1;

	}


	for(y = bmp->binfo.height; y > 0; y--) {
	for(x = 0; x < bmp->binfo.width;  x++) {
	
		// <= 8-Bit
		if(bmp->binfo.count <= 8) {
		if(bmp->binfo.count == 4) {
		
		printf("Not suport BitMAP 4-bit");
		return 2;
		
		}else
		color = *(unsigned int*)((paleta) + *(unsigned char*/*count*/)(((unsigned long)data_area) + i++)) &0xffffff;
		
		// > 8-Bit
		}else if(bmp->binfo.count > 8){



		if(bmp->binfo.count == 24){
	
			color =  (*(unsigned int*)((data_area) + (3*i++))) &0xffffff;

		}else if(bmp->binfo.count == 32){

			color =  (*(unsigned int*)((data_area) + (4*i++))) &0xffffffff;

		}else {
			printf("Not suport BitMAP > 8-bit");
			return 2;
		}
		
	
		}

		// Aqui por onde ampliamos o pixel
		for(int l =0; l < Z00Y; l++) 
		{
			for(int z =0; z < Z00X; z++) 
			{
				if(color != 0)
					put_pixel(X + x*Z00X+z, Y + y*Z00Y+l, w->width, fg, &w->start);
				else
					put_pixel(X + x*Z00X+z, Y + y*Z00Y+l, w->width, bg, &w->start);
			
			}
		}
		
		}}



	return 0;
}

