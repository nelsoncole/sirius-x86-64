#include "bga.h"
#include <mm.h>
#include <stdio.h>
#include <pci.h>
#include <io.h>
#include <gui.h>
#include <driver.h>

void bga_write_register(unsigned short index, unsigned short data)
{
    outportw(VBE_DISPI_IOPORT_INDEX, index);
    outportw(VBE_DISPI_IOPORT_DATA, data);
}
 
unsigned short bga_read_register(unsigned short index)
{
    outportw(VBE_DISPI_IOPORT_INDEX, index);
    return inportw(VBE_DISPI_IOPORT_DATA);
}
 
int bga_is_available(void)
{
    return (bga_read_register(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}
 
void bga_set_video_mode(unsigned int width, unsigned int height, unsigned int bit_depth, int use_LFB, int clean_video_memory)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, width);
    bga_write_register(VBE_DISPI_INDEX_YRES, height);
    bga_write_register(VBE_DISPI_INDEX_BPP, bit_depth);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (use_LFB ? VBE_DISPI_LFB_ENABLED : 0) |
        (clean_video_memory ? 0 : VBE_DISPI_NOCLEARMEM));
}
 
void bga_set_bank(unsigned short bank_number)
{
    bga_write_register(VBE_DISPI_INDEX_BANK, bank_number);
}


int setup_bga(int bus, int slot, int function){

    int width = 800;
    int height = 600;
    int bpp = 32;
    int pitch = width*(bpp/8);

	/*double aspect_ratio = 16.0 / 9.0;
    height = (const int)(width / aspect_ratio);*/

    unsigned long memory = 0;
    unsigned long frame_buffer = 0;//VBE_DISPI_LFB_PHYSICAL_ADDRESS;

    frame_buffer = pci_read_config_dword(bus,slot,function,0x10);
    frame_buffer &= ~0xf;

    mm_mp(frame_buffer, (unsigned long*)&memory, 0x800000, 0);
    
    if(!memory){
        return 1;
    }

    bga_set_video_mode(width , height, bpp, 1, 1);

    update_gui(memory, width, height, pitch, bpp );

    return 0;
}
