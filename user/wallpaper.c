#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <window.h>

#include <time.h>

FILE *f;

int wp(char *name) {

	WINDOW *w = (WINDOW*) (__window);

	//char *wallpaper = (char *) ((unsigned long)&w->start);
	unsigned char *wallpaper = (unsigned char *) malloc(0x800000); // 8 MiB
	int size;

	unsigned int width = 0, height = 0, sig;

	char *_wallpaper = (char*) wallpaper;
	char *p;

	f = fopen(name,"r");
	if(f == NULL) printf("error: fopen\n");
	else {

	
		fseek(f, 0,SEEK_END);
		size = (int) ftell(f);
		fseek(f, 0,SEEK_SET);

		if(fread(wallpaper, 1, size, f) != size) printf("error: fread\n");
		else {

			p = strtok(_wallpaper,"\n ");
			//printf("%s\n",p);

			p = strtok('\0', "\n ");
			width = strtoul (p,NULL, 10);
			p = strtok('\0', "\n ");
			height = strtoul (p,NULL, 10);
			p = strtok('\0', "\n ");
			sig = strtoul (p,NULL, 10);
			_wallpaper = p + strlen(p) + 1;
			
			//printf("width %d height %d sig %d %lx\n",width, height,sig, _wallpaper);


            int x_p = (w->width/2) - (width/2);
            int y_p = (w->height/2) - (height/2);

            if(x_p < 0) x_p = 0;
            if(y_p < 0) y_p = 0;

            int f = width - 800;
            f = f / 2;
			
			char rgb[4];
			for(int y=0;y < height;y++) {

                int count = 0;
				for(int x= 0 ;x < width; x ++) {
					
                    if ( count < width ) {

					    rgb[2] = *_wallpaper++;
					    rgb[1] = *_wallpaper++;
					    rgb[0] = *_wallpaper++;
					    rgb[3] = 0;

					    put_pixel(x + x_p, y + y_p, w->width, *(unsigned int*)&rgb, &w->start);

                        count ++;
                    
                        if( 1  ) {

                           // _wallpaper += 3;
                           // count ++;
                       
                        }
                       
                    }
				}
			}
	
		}

	}


	fclose(f);
	free(wallpaper);
	return 0;
}












