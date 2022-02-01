#include <i965.h>
#include <pci.h>
#include <stdio.h>
#include <gui.h>
#include <string.h>
#include <mm.h>
#include <sleep.h>


i965_t gtt[1];
framebuffer_t fb[2];
mode_t mode[1];

int i965_pci_init(int bus, int dev, int fun)
{	

	unsigned long bar0,bar1,bar2,bar3,bar4;
	
	unsigned data = pci_read_config_dword(bus, dev, fun, 0x00);

	gtt->did = data >>16 &0xffff;
	gtt->vid = data &0xffff;
	
	bar0	= pci_read_config_dword(bus, dev, fun, 0x10) &0xFFFFFFFF;
	bar1  	= pci_read_config_dword(bus, dev, fun, 0x14) &0xFFFFFFFF;
	bar2 	= pci_read_config_dword(bus, dev, fun, 0x18) &0xFFFFFFFF;
	bar3  	= pci_read_config_dword(bus, dev, fun, 0x1C) &0xFFFFFFFF;
	bar4 	= pci_read_config_dword(bus, dev, fun, 0x20) &0xFFFFFFFF;
	
	
#if defined(__x86_64__) || defined(__aarch64__)

	gtt->phy_mmio =  (bar1 << 32 | bar0) &0xFFFFFFFFFFFFFFF0;
	gtt->phy_memory = (bar3 << 32 | bar2) &0xFFFFFFFFFFFFFFF0;
	gtt->iobar = bar4 &0x00000000FFFFFFF0;		
#elif defined(__i386__) || defined(__arm__)
	
	gtt->phy_mmio = bar0 &0xFFFFFFF0;
	gtt->phy_memory = bar2 &0xFFFFFFF0;
	gtt->iobar = bar4 &0xFFFFFFF0;
#else
#error YOUR ARCH HERE
#endif

	
	return 0;
}


int setup_i965(){


	memset(gtt, 0, sizeof(i965_t));

	unsigned int data = pci_scan_class(3);
	
	if(data == -1) {
	
		printf("panic: PCI Display Controller not found!\n");
		return -1;
	}

    printf("Display Controller\n");

	i965_pci_init(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);
	
    /*

        2nd Generation Intel Core Processor Intel HD Graphics (2000/3000)
        Device ID 116
        Vendor ID 8086
        
        3rd Generation Intel Core Processor Intel HD Graphics (2500/4000)
        Device ID 166
        Vendor ID 8086

        Device  ID 156
        Vendor ID 8086

    */
	if((gtt->vid &0xffff) != 0x8086 || (gtt->did != 0x116 && gtt->did != 0x166 && gtt->did != 0x156)  ) { 
		
		printf("Graphic Native Intel, not found, device id %x, vendor id %x\n",gtt->did,gtt->vid);
		
		unsigned long virt_addr;
		mm_mp(gui->frame_buffer, (unsigned long*)&virt_addr, 0x800000/*8MiB*/, 0);
		gui->virtual_buffer = virt_addr;
		
		
		return 1;
	}
	
	
	//Mapear o mmio_base e memória de vídeo
	mm_mp( gtt->phy_mmio, &gtt->mmio_base, 0x100000 /*1MiB*/, 0);
	mm_mp(gtt->phy_memory, (unsigned long*)&gtt->memory, 0x800000/*8MiB*/, 0);

    // clean
    memset( (char*)gtt->memory, 0,  0x800000);
	
	/*char edid[128];
	
	gmbus_read(gtt,0x50, edid, 128);
	
	printf("edid = %s",edid);*/
	
	//disable
	disable_dac(gtt);
	disable_plane(gtt);
	disable_pipe(gtt);
	disable_dpll(gtt);
	disable_legacy_vga_emul(gtt);

	//enable
	enable_dpll(gtt);
	
	// Setup the display timings of your desired mode
	timings(gtt,fb,mode);
	
	gtt->cursor_memory = gtt->memory + 0x700000; // 7MIB
	
	
	enable_pipe(gtt);
	
	// Set a framebuffer address and stride and enable the primary plane and all secondary planes that you wish to use.
	for(int i=0;i<2;i++){
	
		*(unsigned int*)(gtt->mmio_base + 0x70184 + i*0x1000) |= 0; // plane offser
		*(unsigned int*)(gtt->mmio_base + 0x70188 + i*0x1000) |= fb[i].stride; // plane stride
		*(unsigned int*)(gtt->mmio_base + 0x7019c + i*0x1000) |= fb[i].address; // plane address
	
	}
	
	
	//Nelson, espere 1s
	sti();
	sleep(400);
	cli();


	enable_plane(gtt);
	enable_dac(gtt);
	
	// fim
	gtt->INTEL = 1;
	update_gui(gtt->memory, mode->width, mode->height, mode->pitch, mode->bpp );
	
	setup_cursor(gtt);
/*	
	printf("Graphic Native Intel,  mode->width %d, mode->height %d\n", mode->width,mode->height);

    DISPLAY_PIPELINE *pipe = (DISPLAY_PIPELINE*)(gtt->mmio_base + 0x60000 + (0x1000 *0) );

    printf("%d %d %d %d\n",pipe->htotal.active, pipe->vtotal.active,
    pipe->pi_peasrc.h_image_size, pipe->pi_peasrc.v_image_size);

    printf("\n\n%x %x %x\n",*(unsigned int*)(gtt->mmio_base + 0x70184 + 0*0x1000),
    *(unsigned int*)(gtt->mmio_base + 0x70188 + 0*0x1000),
    *(unsigned int*)(gtt->mmio_base + 0x7019c + 0*0x1000) );
	
    for(;;); */

	return 0;
}


void timings(i965_t *driver, framebuffer_t fb[2], mode_t *mode)
{
	char *edid = (char *) (0x500 + 54);
	
	int x = (edid[2] &0xff) | (edid[4] >> 4 &0xf) << 8;
	int y = (edid[5] &0xff) | (edid[7] >> 4 &0xf) << 8;
	//int sx = (edid[12] &0xff) | (edid[14] >> 4 &0xf) << 8;
	//int sy = (edid[13] &0xff) | (edid[14] &0xf) << 8;

	for(int i=0;i < 2; i++) {
	
		// Setup the display timings of your desired mode
	    DISPLAY_PIPELINE *pipe = (DISPLAY_PIPELINE*)(driver->mmio_base + 0x60000 + (0x1000*i) );

        mode->width = x;
        mode->height = y;
        mode->bpp = 32;

        mode->pitch = x;

        while(1) {
            if( !(mode->pitch%16) ) break;
            mode->pitch ++;
        }

         mode->pitch =  mode->pitch * 4;
	
		fb[i].width = mode->width;
		fb[i].height = mode->height;

		fb[i].stride =  mode->pitch;
		fb[i].address = i* 0x400000; //4MiB
	
		pipe->htotal.active = x - 1;
		pipe->vtotal.active = y - 1;

		pipe->pi_peasrc.h_image_size = mode->width  -1;
		pipe->pi_peasrc.v_image_size = mode->height -1;
		
	}
}

void disable_dac(i965_t *driver) 
{
	// ADPA (Analog Display Port A)
	*(unsigned int*)(driver->mmio_base + ADPA) &=~0x80000000;  // disable
	*(unsigned int*)(driver->mmio_base + ADPA) |= 0xc00;	// Monitor off
	
}

void enable_dac(i965_t *driver)
{
	// ADPA (Analog Display Port A)
	*(unsigned int*)(driver->mmio_base + ADPA) |=0x80000000;  // enable
	*(unsigned int*)(driver->mmio_base + ADPA) &=~0xc00;	// Monitor on
}

// Register DSPACNTR
void disable_plane(i965_t *driver)
{	// Display Plane A (Primary A) disable
	*(unsigned int*)(driver->mmio_base + DSPACNTR) &=~0x80000000;

	// Display Plane B (Primary B) disable
	*(unsigned int*)(driver->mmio_base + DSPACNTR + 0x1000) &=~0x80000000;

	// Display Plane C (Primary C) disable
	//*(unsigned int*)(driver->mmio_base + DSPACNTR + 0x2000) &=~0x80000000;
}
void enable_plane(i965_t *driver)
{	// Display Plane A (Primary A) enable
	*(unsigned int*)(driver->mmio_base + DSPACNTR) |=0x80000000; 

	//Display Plane B (Primary B) enable
	*(unsigned int*)(driver->mmio_base + DSPACNTR + 0x1000) |=0x80000000; 

	//Display Plane C (Primary C) enable
	//*(unsigned int*)(driver->mmio_base + DSPACNTR + 0x2000) |=0x80000000;
}

// Register PIPEACONF
void disable_pipe(i965_t *driver)
{	// Pipe A Configuration Register
	*(unsigned int*)(driver->mmio_base + PIPEACONF) &=~0xc0000000;  // disable
	// Pipe B Configuration Register
	*(unsigned int*)(driver->mmio_base + PIPEACONF + 0x1000) &=~0xc0000000; // disable
}

void enable_pipe(i965_t *driver)
{	// Pipe A Configuration Register
	*(unsigned int*)(driver->mmio_base + PIPEACONF) |= 0xc0000000; // enable
	// Pipe B Configuration Register
	*(unsigned int*)(driver->mmio_base +  PIPEACONF + 0x1000) |= 0xc0000000; // enable
}

//
void disable_dpll(i965_t *driver)
{

}

void enable_dpll(i965_t *driver)
{

	*(unsigned int*)(driver->mmio_base + DPLLA_CNTL) = 0x94020C00;

}
// Registro VGACNTRL
void disable_legacy_vga_emul(i965_t *driver)
{	unsigned char data = inpb(VGA_CLOCKING_MODE_CTRL)  | 0x20; // set bit 5 screen off
	outpb(VGA_CLOCKING_MODE_CTRL,data);
	// VGA plane control disable
	*(unsigned int*)(driver->mmio_base + VGA_PLANE_CTRL) |=0x80000000;
	// VGA Centering Enable
	*(unsigned int*)(driver->mmio_base + VGACNTRL) &=~0x3000000;
	// VGA Display Disable
	*(unsigned int*)(driver->mmio_base + VGACNTRL) |=0x80000000;
}


// GMBUS functions
void gmbus_reset(i965_t *driver)
{  	
   	/**(unsigned int*)(driver->mmio_base + GMBUS1) = 0;
   	
   	gmbus_enable_writeprot(driver);
   	gmbus_disable_writeprot(driver);
   	*/
}

void gmbus_stoptransaction(i965_t *driver)
{
	*(unsigned int*)(driver->mmio_base + GMBUS1) = (1 << 30) | (1 << 27);
}

void gmbus_wait(i965_t *driver)
{
	while( ! ( *(unsigned int*)(driver->mmio_base + GMBUS2) & (1 << 11) ) );
}

void gmbus_read(i965_t *driver,unsigned int offset, void *buffer, int size)
{
	int i;
 	unsigned int gmbus0_val = 0;
   	unsigned int gmbus1_val = 0;
   	
   	unsigned char *buf = (unsigned char *)buffer;
   	
   	// SW Ready
   	// WAIT
   	// INDEX
   	// READ
   	gmbus1_val = (1 << 30) | (1 << 25) | (1 << 26) | (size & 511) << 16 | ((offset & 127) << 1) | 1;
   	
   	gmbus_reset(driver);
   	
   	*(unsigned int*)(driver->mmio_base + GMBUS0) = gmbus0_val;
   	*(unsigned int*)(driver->mmio_base + GMBUS2) = gmbus1_val;

	 for (i = 0; i < size; i += 4) {
       
       	gmbus_wait(driver);
       	unsigned int bytes = *(unsigned int*)(driver->mmio_base + GMBUS3);
       	
       	buf[i] = bytes & 0xFF;
       	if (size > i + 1) buf[i + 1] = (bytes >> 8) & 0xFF;
       	if (size > i + 2) buf[i + 2] = (bytes >> 16) & 0xFF;
       	if (size > i + 3) buf[i + 3] = (bytes >> 24) & 0xFF;
   	}

   	gmbus_stoptransaction(driver);

}


void gmbus_write(unsigned int offset, const void *buffer, int size)
{


}
