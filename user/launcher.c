#include <window.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <data.h>
#include <time.h>

#include <pipe.h>

#include <math.h>
#include <stdbool.h>

#include <sys/communication.h>

#define APP_LIST_SIZE 1024
char *app_memory;

typedef struct _PAINT
{
	unsigned long 	w;
	unsigned long	spinlock;
	struct _PAINT   *next;
}__attribute__ ((packed)) PAINT;


struct app
{
    unsigned long id;
    unsigned long status;
    unsigned int x1, y1, x2, y2; 
    unsigned long icone_addr; 
    unsigned long path_name_addr;

    //
    unsigned long w; 
}__attribute__ ((packed));



PAINT *paint, *paint_ready_queue;
struct app app[APP_LIST_SIZE];
int app_index;
int app_index_foco;
unsigned long app_id;
int app_x, app_y;
int app_width, app_height;

char *app_pathname;
extern char *pwd;

extern int wp(char *name);
static void update(const char *id, WINDOW *w );

// Fixar app na bara de tarefa
static int app_fixe(unsigned long status, const char *pathname, const char *icone_pathname, WINDOW *w)
{
    app[app_index].id = 0;
    app[app_index].status |= status;


    app[app_index].x1 = app_x + (app_index *(32 + 4));
    app[app_index].y1 = app_y;
    app[app_index].x2 = app_width;
    app[app_index].y2 = app_height;

    // 16 KiB
    unsigned long addr = (unsigned long) malloc(0x4000);
    app[app_index].path_name_addr = addr;
    app[app_index].icone_addr = addr + 0x1000;

    char *pa = (char*) app[app_index].path_name_addr; 
    strcpy(pa, pathname);

    char *data = (char*) app[app_index].icone_addr;

    // open icone
    FILE *f = fopen(icone_pathname,"r");
	if(f){
	
		fseek(f,0,SEEK_END);
		int size = ftell(f);
		rewind(f);
	

        if(size <= 0x3000) {
		    fread(data, 1, size, f);

		    BitMAP( data, app[app_index].x1 , app[app_index].y1, 0x404040, w);
	    }

		fclose(f);

	}else printf("open: %s error\n", icone_pathname );


    return app_index ++;
}

static int app_foreground(int x, int y)
{
    WINDOW *wa = 0;
    paint = paint_ready_queue;

    while(paint) {
        WINDOW *w = (WINDOW *) paint->w;
        paint = paint->next;
        if( !w ) continue;

        int x1 = w->pos_x;
        int y1 = w->pos_y;
        int x2 = w->width;
        int y2 = w->height;

        if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2) ) 
        {
            wa = w;
        }
    }

    if(!wa) return (-1);

    for(int i=0; i < app_index; i++ ) {
        WINDOW *wb = (WINDOW *) app[i].w;
        if( wa == wb && (app[i].status & 1)  ) {
            return i;
        }
    }

    return (-1);
}


static int app_check_cursor()
{
    int x, y;
    int r = -1;

    if(mouse->b&0x1) 
	{
        x = mouse->x;
        y = mouse->y;

        for(int i=0; i < app_index; i ++) {
            int x1 = app[i].x1;
            int y1 = app[i].y1;
            int x2 = app[i].x2;
            int y2 = app[i].y2;

            if( x > x1 && y > y1 && x < (x1+x2) && y < (y1 + y2) ) 
            {
                r = i;
                break;
            }
        }

        if(r == -1) {
            r = app_foreground( x, y );
        } 
                
    }
    // TODO
    //while(mouse->b&0x1)__asm__ __volatile__("pause": : :"memory");
    return (r);
}

static void app_remove_foco(WINDOW *w)
{
    if(app_index_foco == -1)
    {   
        return;
    }

    // remove foco
    app[app_index_foco].status &=~0x2;

    char *data = (char*) app[app_index_foco].icone_addr;

    BitMAP( data, app[app_index_foco].x1 , app[app_index_foco].y1, 0x404040, w);

    app_index_foco = -1;
}

static int app_execute(int index, WINDOW *w)
{
    char *data = (char*) app[index].icone_addr;

    if(app[index].status & 0x1)
    {
        if(app[index].status & 0x2) 
        {   // tem o foco 
            return 0;
        }else {            
            // foco
            app[index].status |= 0x2;
            app_remove_foco(w);
            app_index_foco = index;
            BitMAP( data, app[index].x1 , app[index].y1, 0xe0e0e0, w);

            // foco
            //TODO 
            //__asm__ __volatile__("int $0x72"::"d"(8),"S"( app[index].id),"c"(5));
            struct communication commun, commun2;
            commun.type = COMMUN_TYPE_FOCO;
            commun.pid = 0;
            commun.apid = app[index].id;
            communication(&commun, &commun2);

        }
    }
    else {
        app[index].id = app_id;
        app[index].status |= 0x1;

        app[index].status |= 0x2;
        app_remove_foco(w);
        app_index_foco = index;
        BitMAP( data, app[index].x1 , app[index].y1, 0xe0e0e0, w);

        strcpy( app_pathname , (char*)app[index].path_name_addr );


        FILE *fp = fopen(app_pathname,"r+b");
	    if(fp) {

            fseek(fp,0,SEEK_END);
	        int size = ftell(fp);
	        rewind(fp);
            fread (app_memory, 1, size, fp);
		    fclose(fp);
	    }else {
		    printf("fopen error: \'%s\'\n", app_pathname);
		    return -1;
	    }

        // execute
		__asm__ __volatile__( "movq %%rax,%%r8;"
		" int $0x72;"
		::"d"(8),"D"(app_memory),"S"( app_id ),"c"(2), "a"(pwd));

        app_id ++;
    }

    return 0;
}


static void app_fixe_remove(WINDOW *w, unsigned long id)
{
    for(int i = 0; i < app_index; i ++)
    {
        if(app[i].id == id) {
            if(app[i].status & 0x2) app_remove_foco(w);
    
            app[i].id = 0;
            app[i].status = 0;

            break;
        }
    }
}

static void app_window_register(__pipe_t *pipe)
{
    unsigned long new_window = pipe->r4 ;
    unsigned long id = 0;
    id = pipe->r1 | (pipe->r2 << 16) | (pipe->r3) << 32;

    for(int i=0; i < app_index; i ++) {
        if( app[i].id == id ) 
        {
            app[i].w = new_window;
            // enviar para compositor
	        __asm__ __volatile__("int $0x72"::"d"(2),"D"(new_window));

            return;
        }
    }

    printf("error: app_window_register\n");

}

const char *ss[60]={\
"00","01","02","03","04","05","06","07","08","09",
"10","11","12","13","14","15","16","17","18","19",
"20","21","22","23","24","25","26","27","28","29",
"30","31","32","33","34","35","36","37","38","39",
"40","41","42","43","44","45","46","47","48","49",
"50","51","52","53","54","55","56","57","58","59"
};



static void background(int width, void *frontbuffer);
int main()
{
    // register pipe launcher
    __asm__ __volatile__("int $0x72"::"d"(11));

    app_memory = (char*)malloc(0x80000);
    app_pathname = malloc(0x1000);
    app_index = 0;
    app_index_foco = -1;
    app_id = 1;
    memset(app, 0, sizeof(struct app) *APP_LIST_SIZE);


	time_t	timer;
	struct tm *h;
	char s[32];
	memset(s,0,32);
	
	int min = 0;
	
	h = gmtime(&timer);
	sprintf(s, "%s:%s",ss[h->tm_hour], ss[h->tm_min]);

	WINDOW *w = (WINDOW*) (__window);
	memset((char*)&w->start,0, 0x400000-0x1000);
	
	w->width = w->rx;
	w->height = w->ry;
	w->pos_x = 0;
	w->pos_y = 0;
	
	
	w->area_width = w->width;
	w->area_height = w->height;
	w->area_x = 0;
	w->area_y = 0;
	
	//init font
	w->font.x = 8;
	w->font.y = 16;
	w->font.fg_color = 0;//0xfcfc00;
	w->font.bg_color = 0xe0e0e0;
	char *tmp = (char*) malloc(FONT_DATA_SIZE);
	memcpy(tmp, font8x16, FONT_DATA_SIZE);
    w->font.buf = (unsigned long)tmp;

    w->fg = w->font.fg_color;
	w->bg = w->font.bg_color;
	w->text_fg = 0;
    w->cy = w->cx = 0;

    
    // area 8ba8aa
	drawline(w->pos_x ,w->pos_y, w->width, w->height, 0x2f2f,w);
    //wp("w.ppm");
    background(w->width, (char*)&w->start);

	// barra
	drawline(w->pos_x ,w->height-36, w->width, WMENU_BAR_SIZE, 0x404040,w);

	// register 
	//wcl(w);
    __asm__ __volatile__("int $0x72"::"d"(2),"D"(w));

    // get list
    unsigned long addr = 0;
    __asm__ __volatile__("int $0x72":"=a"(addr):"d"(13));
    paint = paint_ready_queue = (PAINT*)addr;

    // inicializa os icones fixados
    app_x = w->pos_x  +  4; 
    app_y = w->height - 34;
    app_width = 32;
    app_height = 28;

    app_fixe( 0, "explore.bin\0" , "folder.bmp\0" , w);
    app_fixe( 0, "term.bin\0" , "trm.bmp\0" , w);
    app_fixe( 0, "editor.bin\0" , "edit.bmp\0" , w);
	
	for(;;){

        int index = app_check_cursor();
        if(index != -1) {
            app_execute(index, w);
        }
	
	
		if(min != h->tm_min) {
			min = h->tm_min;
			sprintf(s, "%s:%s",ss[h->tm_hour], ss[h->tm_min]);
			update(s, w );
		}

        __pipe_t pipe;

        if( pipe_read_2x ( &pipe, __pipe__) )
        {
            switch(pipe.id) {
                case PIPE_EXIT:
                    app_fixe_remove(w, pipe.r4);
                break;
                case PIPE_WINDOW_REGISTER:
                    app_window_register(&pipe);
                break;
                default:
                    printf("failed comunication\n");
                break;
            }
        }
		
	}

	return 0;
}

static void update(const char *id, WINDOW *w )
{
	
	int _x = w->area_x + w->width - 80;
	int _y = w->area_y + w->height- 28;
	
	int width  = 10*8; 
	int height = 20;
	
	int fg = 0xFFFFFF;//-1; 
	int bg = 0x404040;//0x80808080;
	
	//drawline(_x, _y, width, height, bg, w);
	//drawrect(_x, _y, width, height, bg - 0x10101010, w );
	
	
	// centro
	int len = strlen(id);
	drawstring(id, _x + ( width/2 - (len*8/2)), _y + (height/2 - 8), fg, bg, &w->font, w);
	
}

/*
 *
 *
 */

struct vec3
{
	double x;
	double y;
	double z;
};

struct color
{
        double x;
        double y;
        double z;
};

struct ray
{
	struct vec3 origin;
	struct vec3 direction;
};

struct vec3 vec3(double x, double y, double z){
	struct vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

// norma
double normsquared(struct vec3 *v){
      	double x = ((v->x*v->x) + (v->y*v->y) + (v->z*v->z));
	return x;
}

double norm(struct vec3 *v){
	return sqrt(normsquared(v));
}


// operações
struct vec3 divec3(struct vec3 *v, double t){
	struct vec3 r;
	r.x = v->x / t;
	r.y = v->y / t;
	r.z = v->z / t;

	return r;
}

struct vec3 mulvec3(struct vec3 *v, double t){
        struct vec3 r;
        r.x = v->x*t;
        r.y = v->y*t;
        r.z = v->z*t;
        return r;
}

struct vec3 sumvec3(struct vec3 *v1, struct vec3 *v2){
        struct vec3 r;
        r.x = v1->x + v2->x;
        r.y = v1->y + v2->y;
        r.z = v1->z + v2->z;
        return r;
}

struct vec3 subvec3(struct vec3 *u, struct vec3 *v){
        struct vec3 r;
        r.x = u->x - v->x;
        r.y = u->y - v->y;
        r.z = u->z - v->z;
        return r;
}



// produto escalar
double dot(struct vec3 *u, struct vec3 *v){
	double r = (u->x*v->x);
	r += (u->y*v->y);
	r += (u->z*v->z);
	return r;
}


//vector unitario
struct vec3 unitvector(struct vec3 *v){
	struct vec3 r = divec3(v, norm(v));
	return r;
}

struct ray ray(struct vec3 *orig, struct vec3 *dir){
	struct ray r;
	r.origin.x = orig->x;
	r.origin.y = orig->y;
	r.origin.z = orig->z;

	r.direction.x = dir->x;
	r.direction.y = dir->y;
	r.direction.z = dir->z;

	return r;
}

struct vec3 rayat(double t, struct ray *r){
	struct vec3 v;
	v = mulvec3(&r->direction, t);
	v = sumvec3(&r->origin, &v);
	return v;
}

struct color vecolor(double x, double y, double z){
	struct color c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

struct color mulcolor(struct color *u, double t){
	struct color c;
	c.x = u->x*t;
    c.y = u->y*t;
    c.z = u->z*t;
	return c;
}

struct color sumcolor(struct color *u, struct color *v){
        struct color c;
        c.x = u->x+v->x;
        c.y = u->y+v->y;
        c.z = u->z+v->z;
        return c;
}

double hit_sphere(struct vec3 *center, double radius, struct ray *r){
        struct vec3 oc = subvec3(&r->origin, center);
        double a = dot(&r->direction, &r->direction);
        double b = (double)2.0*dot(&oc, &r->direction);
        double c = dot(&oc, &oc) - radius*radius;
        double discriminant = b*b - 4*a*c;
        if (discriminant < 0) {
        	return -1.0;
    	} else {
        	double x =( (- b - sqrt(discriminant))/ a);
		if(x < 0.0) x = x*(-1.0);
		return x;
    	}
}

struct color ray_color(struct ray *r) {
    double t;
    /*struct vec3 center =vec3(0.0, 0.0, -1.0);
	t = hit_sphere(&center, 0.5, r);
	if (t > (double)0.0){
		struct vec3 u = rayat(t, r);
		struct vec3 v = subvec3(&u, &center);
		struct vec3 n =unitvector(&v);
		struct color c1 = vecolor(1.0, 1.0, 1.0);
		struct color c2 = vecolor(n.x,n.y,n.z);
		c1 = sumcolor(&c1, &c2);
		c1 = mulcolor(&c1, 0.5);
        	return c1;

	}*/

    struct vec3 unit_direction = unitvector(&r->direction);
    t = 0.5*(unit_direction.y + 1.0);
	struct color a = vecolor(1.0, 1.0, 1.0);
	a = mulcolor(&a, (double)1.0-t);
	struct color b = vecolor(0.5, 0.7, 1.0);
	b = mulcolor(&b, t);

    return sumcolor(&a, &b);
}

unsigned int write_color(struct color *c){
	unsigned char rgb[4];

    rgb[0] = (unsigned char)(255.999 * c->z);
    rgb[1] = (unsigned char)(255.999 * c->y);
    rgb[2] = (unsigned char)(255.999 * c->x);
	rgb[3] = 0;
	return *(unsigned int*)&rgb;
}



static void background(int width, void *frontbuffer){

    // Image
	double aspect_ratio = 16.0 / 9.0;

    const int image_width = width;
    if(image_width == 800 || image_width == 1024){
        aspect_ratio = 4.0 / 3.0;
    }
    const int image_height = (const int)(image_width / aspect_ratio);

	// Camera
	double viewport_height = 2.0;
	double viewport_width = aspect_ratio * viewport_height;
	double focal_length = 1.0;

	struct vec3 origin = vec3(0.0, 0.0, 0.0);
	struct vec3 horizontal = vec3(viewport_width, 0.0, 0.0);
	struct vec3 vertical = vec3(0.0,viewport_height, 0.0);


	struct vec3 vector1 = divec3(&horizontal, 2);
	vector1 = subvec3(&origin, &vector1);
	struct vec3 vector2 = vec3(0, 0, focal_length);
	struct vec3 vector3 = divec3(&vertical, 2);
	vector2 = subvec3(&vector3, &vector2);

	struct vec3 lower_left_corner =subvec3(&vector1, &vector2);

    // Render
    int y=0;
	for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double u = (double)i / (image_width-1);
            double v = (double)j / (image_height-1);
			vector1 = mulvec3(&horizontal, u);
			vector1 = sumvec3(&lower_left_corner, &vector1);
			vector2 = mulvec3(&vertical, v);
			vector1 = sumvec3(&vector1, &vector2);
			struct vec3 dir = subvec3(&vector1, &origin);
			struct ray r = ray(&origin, &dir);
          	struct color pixel_color = ray_color(&r);
			unsigned int rgb = write_color(&pixel_color);
            put_pixel( i, y, width, rgb, frontbuffer);
        }

        y ++;
    }
}
