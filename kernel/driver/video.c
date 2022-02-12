#include <i965.h>
#include <pci.h>
#include <mm.h>
#include <gui.h>
#include <stdio.h>
#include <driver.h>


extern int setup_bga(int bus, int slot, int function);

void initialize_graphic_adapter(){

    unsigned int data = pci_scan_class(3);
	
	if(data == -1) {
	
		printf("panic: PCI Display Controller not found!\n");
		return;
	}

	int bus = data  >>  24 &0xff;
    int slot = data  >> 16  &0xff;
    int function = data &0xffff;
    unsigned short device, vendor;
    int no = 0;

    data = pci_read_config_dword(bus,slot,function,0);
    device = (data >> 16) &0xFFFF;
    vendor = data &0xFFFF;


    if( vendor == 0x8086) {
       no = setup_i965(bus, slot, function);

    } else if((vendor == 0x1234 && device == 0x1111) || (vendor == 0x80EE && device == 0xBEEF) ){
        no = setup_bga(bus, slot, function);

    } else {
        no = 1;
        printf("Unknown Graphic device\n");
        printf("Other Graphic device id %x vendor id %x\n", device, vendor);
    }

    if(no != 0){

        unsigned long virt_addr;
		mm_mp(gui->frame_buffer, (unsigned long*)&virt_addr, 0x800000/*8MiB*/, 0);
		gui->virtual_buffer = virt_addr;
    }
}
