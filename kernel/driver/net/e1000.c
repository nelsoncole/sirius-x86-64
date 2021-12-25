#include <ethernet.h>
#include <pci.h>
#include <string.h>
#include <mm.h>
#include <sleep.h>
#include <stdio.h>
#include <irq.h>
#include "e1000.h"

static unsigned long base_addr;
static unsigned char mac_address[6];
static unsigned volatile long e1000_package_recieved_ack = 0;

static int rx_cur;
static int tx_cur;
struct e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC];
struct e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC];

struct e1000_rx_memory rx_memory;
struct e1000_rx_memory tx_memory;

int e1000_send_package(ethernet_package_descriptor_t desc);
ethernet_package_descriptor_t e1000_recieve_package();


unsigned int e1000_read_command(unsigned short addr){
    return *( (volatile unsigned int *)(base_addr + addr));
}

void e1000_write_command(unsigned short addr,unsigned int val){
    *( (volatile unsigned int *)(base_addr + addr)) = val;
}


unsigned char e1000_is_eeprom(){
    for(int i = 0 ; i < 1000 ; ++i){
        unsigned long to = e1000_read_command(0x14);
        if((to&0x10)==0x10){
            return 1;
        }
    }
    return 0;
}

void e1000_enable_int(){
    e1000_write_command(0xD0,0x1F6DC);
    e1000_write_command(0xD0,0xff & ~4);
    e1000_read_command(0xC0);
    printf("[E1000] Interrupts enabled!\n");
}

void e1000_link_up(){
    unsigned long ty = e1000_read_command(0);
    e1000_write_command(0, ty | 0x40);
    printf("[E1000] Link is up!\n");
}

void irq_e1000(){
	//printf("[E1000] Interrupt detected\n");
    e1000_write_command(0xD0,1);
    unsigned int to = e1000_read_command(0xC0);
    if(to&0x01){
        printf("[E1000] Transmit completed!\n");
    }else if(to&0x02){
        printf("[E1000] Transmit queue empty!\n");
    }else if(to&0x04){
        printf("[E1000] Link change!\n");
        // Set Link Up
        unsigned long ty = e1000_read_command(0);
        e1000_write_command(0, ty | 0x40);
    }else if(to&0x80){
        printf("[E1000] Package recieved!\n");
		((unsigned volatile long*)((unsigned volatile long)&e1000_package_recieved_ack))[0] = 1;

    }else if(to&0x10){
        printf("[E1000] Good threshold!\n");
    }else{
        printf("[E1000] Unknown interrupt: %x !\n",to);
    }
}

void init_e1000(int bus,int slot,int function){
    printf("[E1000] E1000 initialised bus=%x slot=%x function=%x \n",bus,slot,function);
    base_addr = pci_read_config_dword(bus,slot,function,0x10) & 0xFFFFFFFE;
    printf("[E1000] Base physical address: %x \n",base_addr);
	unsigned long usbint = pci_read_config_dword(bus,slot,function,0x3C) & 0x000000FF;
    printf("[E1000] USBINT %d \n",usbint);
    
    if(usbint == 9) {
        // TODO improviso para VirtualBox
        // IRQ19 
        usbint += 10;
    }

    // Configurar IRQ Handler
    fnvetors_handler[usbint] = &irq_e1000;

    // Mapear para virtual o endereço fisico
    unsigned long virt_addr;
	mm_mp(base_addr, (unsigned long*)&virt_addr, 0x400000/*4MiB*/, 0);
    base_addr = virt_addr;

     // Alloc memory
    // Set virtual address
    // Get physical address
    // 8KiB por descritor
    alloc_pages(0, 66, (unsigned long*)&virt_addr); //256 + 8 = 304KIB
    rx_memory.vmem = virt_addr;
    rx_memory.phymem = get_phy_addr(rx_memory.vmem);
    rx_memory.descsize = 0x2000;
    rx_memory.blocksize = 0x2000; // 8KiB
    rx_memory.start = rx_memory.vmem + rx_memory.descsize;
    alloc_pages(0, 18, (unsigned long*)&virt_addr); //64 + 8 = 72KiB
    tx_memory.vmem = virt_addr;
    tx_memory.phymem = get_phy_addr(tx_memory.vmem);
    tx_memory.descsize = 0x2000;
    tx_memory.blocksize = 0x2000; // 8KiB
    tx_memory.start = tx_memory.vmem + tx_memory.descsize;

    // limpar memória
    memset((char*)rx_memory.vmem, 0, 66*0x1000);
    memset((char*)tx_memory.vmem, 0, 18*0x1000);

    if( e1000_is_eeprom() ){

        printf("[E1000] Device has EEPROM\n");
        printf("[E1000] EEPROM is not supported, yet\n");
        return;

    }else{

        unsigned int data = e1000_read_command(0x5400);
        mac_address[0] = ((data & 0x000000FF)>>0) & 0xFF;
        mac_address[1] = ((data & 0x0000FF00)>>8) & 0xFF;
        mac_address[2] = ((data & 0x00FF0000)>>16) & 0xFF;
        mac_address[3] = ((data & 0xFF000000)>>24) & 0xFF;
        data = e1000_read_command(0x5400 + 4);
        mac_address[4] = ((data & 0x000000FF)>>0) & 0xFF;
        mac_address[5] = ((data & 0x0000FF00)>>8) & 0xFF;
    }
    printf("[E1000] MAC: %x:%x:%x:%x:%x:%x \n",mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);


    // Habilitar o PCI Busmastering DMA
    if(!(pci_read_config_dword(bus,slot,function,0x04)&0x04))
    {
        unsigned int to = pci_read_config_dword(bus,slot,function,0x04) | 0x04;
        pci_write_config_dword(bus,slot,function,0x04, to);

        printf("[E1000] Busmastering was not enabled, but now it is!\n");
    }

    // No multicast
    // MTA(n) = 0
    for(int i = 0; i < 0x80; i++){
        e1000_write_command(0x5200 + (i * 4), 0);
    }

    // Desabilitar Interrupção
    e1000_write_command(0xD0,0x1F6DC);
    e1000_read_command(0xC0);

    // Initialize Receive
    struct e1000_rx_desc *descs = (struct e1000_rx_desc *) rx_memory.vmem;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        rx_descs[i] = descs++;

        unsigned long long addr = rx_memory.phymem + rx_memory.descsize + (rx_memory.blocksize*i);
        rx_descs[i]->addr_1 = addr;
        rx_descs[i]->addr_2 = addr >> 32;
        rx_descs[i]->status = 0;
        rx_descs[i]->length = rx_memory.blocksize;
    }

    e1000_write_command(REG_RXDESCLO, (unsigned int) rx_memory.phymem);
    e1000_write_command(REG_RXDESCHI, (unsigned int) (rx_memory.phymem >> 32));
    e1000_write_command(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);
    e1000_write_command(REG_RXDESCHEAD, 0);
    e1000_write_command(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);

    e1000_write_command(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);

    rx_cur = 0;

    // Initialize Transmit
    struct e1000_tx_desc *descs2 = (struct e1000_tx_desc *) tx_memory.vmem;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        tx_descs[i] = descs2++;

        unsigned long long addr = tx_memory.phymem + tx_memory.descsize + (tx_memory.blocksize*i);
        tx_descs[i]->addr_1 = addr;
        tx_descs[i]->addr_2 = addr >> 32;
        tx_descs[i]->cmd = 0;
        tx_descs[i]->length = 0;
        tx_descs[i]->status = TSTA_DD;
    }

    e1000_write_command(REG_TXDESCLO, (unsigned int) tx_memory.phymem );
    e1000_write_command(REG_TXDESCHI, (unsigned int) (tx_memory.phymem >> 32));
    e1000_write_command(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);
    e1000_write_command( REG_TXDESCHEAD, 0);
    e1000_write_command( REG_TXDESCTAIL, 7);

    e1000_write_command(0x3828,  (0x01000000 | 0x003F0000));
    e1000_write_command(REG_TCTRL,  ( 0x00000ff0 | 0x003ff000 | 0x8 | 0x2));
    e1000_write_command(REG_TIPG,  0x0060200A);

    tx_cur = 0;

    // set interrupts
    e1000_enable_int();

    // SLU (bit 6) - Set Link Up
    e1000_link_up();

    // register driver
    register_ethernet_device((unsigned long)&e1000_send_package,(unsigned long)&e1000_recieve_package,mac_address);
}

int e1000_send_package(ethernet_package_descriptor_t desc){

    unsigned long long dest = tx_memory.start + (tx_memory.blocksize*tx_cur);

    if(desc.buffersize > tx_memory.blocksize){
        printf("Package size is error\n");
        return 2;
    }

    memcpy((char*)dest, desc.buf, desc.buffersize);

    tx_descs[tx_cur]->length = desc.buffersize;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    tx_descs[tx_cur]->status = 0;

    unsigned char old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;

    e1000_write_command(REG_TXDESCTAIL, tx_cur);

    int to = 0;
    while(!(tx_descs[old_cur]->status & 0xff)){

        sleep(10);
        to++;
        if(to>500){
            printf("No send package\n");
            return 1;
        }
    }     
   

    return 0;
}

ethernet_package_descriptor_t e1000_recieve_package(){
    ethernet_package_descriptor_t desc;

    while(1){
        for(int i = 0 ; i < E1000_NUM_RX_DESC; i++){

            if((rx_descs[i]->status & 0x1))
            {
                unsigned long long addr = rx_memory.start + (rx_memory.blocksize*i);
                unsigned short len = rx_descs[i]->length;

                desc.buffersize = len;
                desc.buf = (void*) addr;

                rx_descs[i]->status &= ~1;
                e1000_write_command(REG_RXDESCTAIL, i );
                return desc;
            }
        }
    }

}

