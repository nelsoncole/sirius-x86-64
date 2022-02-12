#ifndef __I965_H
#define __I965_H

#include <gui.h>

// GMBUS registers
#define GMBUS0 0x5100 //GMBUS0—GMBUS Clock/Port Select
#define GMBUS1 0x5104 //GMBUS1—GMBUS Command/Status
#define GMBUS2 0x5108 //GMBUS2—GMBUS Status Register
#define GMBUS3 0x510C //GMBUS3—GMBUS Data Buffer

// Display Clock Control Registers (06000h–06FFFh)
#define DPLLA_CNTL	0x06014
#define DPLLB_CNTL	0x06018
#define ADPA 		0x61100

// Display Pipeline A
#define PIPEA_DSL	0x70000
#define PIPEA_SLC	0x70004
#define PIPEACONF 	0x70008

//Display A (Primary) Plane Control
#define DSPACNTR	0x70180
#define DSPALINOFF	0x70184
#define DSPASTRIDE	0x70188
#define DSPASURF	0x7019C
#define DSPATILEOFF	0x701A4

//VGA Plane Control registers
// Bit 31 = plane control disable
// Bit 29 = plane pipe select
#define VGA_PLANE_CTRL 	0x41000

//VGA control registers
#define VGACNTRL 	0x71400
// SR01⎯Clocking Mode
#define VGA_CLOCKING_MODE_CTRL 0x3C5	

typedef struct _framebuffer {

	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int address;
	
}__attribute__ ((packed)) framebuffer_t;


typedef struct _i965 {
	unsigned short did, vid;
	unsigned long phy_mmio, phy_memory, iobar;
	unsigned long mmio_base, memory, cursor_memory;
	int INTEL;
	 	
}__attribute__ ((packed)) i965_t;

typedef struct _TOTAL {
	//Pipe Horizontal Total Register
	unsigned int 	active :12;
	unsigned int 	rsv1 :4;
	unsigned int	total :13;
	unsigned int	rsv2 :3;
}__attribute__ ((packed)) TOTAL;

typedef struct _BLANK {
	// Pipe Blank Register
	unsigned int 	start :13;
	unsigned int 	rsv1 :3;
	unsigned int	end :13;
	unsigned int	rsv2 :3;
}__attribute__ ((packed)) BLANK;
typedef struct _SYNC {
	// Pipe Sync Register
	unsigned int 	start :13;
	unsigned int 	rsv1 :3;
	unsigned int	end :13;
	unsigned int	rsv2 :3;
}__attribute__ ((packed)) SYNC;
typedef struct _PIPEASRC {
	// Pipe Source Image Size
	unsigned int 	v_image_size :12;
	unsigned int 	rsv1 :4;
	unsigned int	h_image_size :12;
	unsigned int	rsv2 :4;	
}__attribute__ ((packed)) PIPEASRC;
typedef struct _BCLRPAT {
	// Pipe Border Color Pattern Register
	unsigned int 	border_blue :8;
	unsigned int 	border_green :8;
	unsigned int	border_red :8;
	unsigned int	rsv :8;	
}__attribute__ ((packed)) BCLRPAT;
typedef struct _VSYNCSHIFT {
	// Vertical Sync Shift Register
	unsigned int 	shift :13;
	unsigned int 	rsv :19;
}__attribute__ ((packed)) VSYNCSHIFT;

typedef struct _CRCCTRLRED {
	// Pipe CRC Color Channel Control Register (Red)
	unsigned int	value :23;
	unsigned int	rsv :5;
	unsigned int	select :3;
	unsigned int	crc :1;

}__attribute__ ((packed)) CRCCTRLRED;

typedef struct _CRCCTRLX {
	// Pipe CRC Color Channel Control Register  (Green, Blue, Residual)
	unsigned int	value :23;
	unsigned int	rsv :9;
}__attribute__ ((packed)) CRCCTRLX;

typedef struct _CRCRESX {
	//  Pipe CRC Color Channel Result Register (Red, Green, Blue, Residual1)
	unsigned int	value :23;
	unsigned int	rsv :9;
}__attribute__ ((packed)) CRCRESX;

// Display Engine Pipeline Registers (60000h–6FFFFh)
typedef volatile struct _DISPLAY_PIPELINE {

	TOTAL 	htotal;
	BLANK 	hblank;
	SYNC 	hsynic;
	TOTAL	vtotal;		
	BLANK	vblank;
	SYNC 	vsynic;
	unsigned int 	rsv1;
	PIPEASRC	pi_peasrc;
	BCLRPAT	bclrpat;
	unsigned int	rsv2;
	VSYNCSHIFT 	vsyncshift;
	unsigned int	rsv3;
	CRCCTRLRED	crc_red_control;
	CRCCTRLX	crc_green_control;
	CRCCTRLX	crc_blue_control;
	CRCCTRLX	crc_residual_control;

	CRCRESX	crc_red_result;
	CRCRESX	crc_green_result;
	CRCRESX	crc_blue_result;
	CRCRESX	crc_residual_result;
	//end
}__attribute__ ((packed)) DISPLAY_PIPELINE;


extern i965_t gtt[1];


void disable_dac(i965_t *driver);
void enable_dac(i965_t *driver);
void disable_plane(i965_t *driver);
void enable_plane(i965_t *driver);
void disable_pipe(i965_t *driver);
void enable_pipe(i965_t *driver);
void disable_dpll(i965_t *driver);
void enable_dpll(i965_t *driver);
void disable_legacy_vga_emul(i965_t *driver);

void timings(i965_t *driver, framebuffer_t fb[2], mode_t *mode);

void gmbus_read(i965_t *driver,unsigned int offset, void *buffer, int size);

void setup_cursor(i965_t *dev);
void update_graphic_cursor(int x, int y );
int setup_i965(int bus, int slot, int function);



#endif
