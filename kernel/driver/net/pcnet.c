#include <stdio.h>
#include <string.h>
#include <ethernet.h>
#include <pci.h>
#include <io.h>
#include <mm.h>
#include <irq.h>
#include <kernel.h>
#include "pcnet.h"

static unsigned int io_base;
struct pcnet_softc pcnet_softc;
struct pcnet_block *pcnet_block;
static void irq_pcnet();
static void pcnet_pci(int bus, int slot, int function);
ethernet_package_descriptor_t *pcnet_recieve_package();
int pcnet_send_package(ethernet_package_descriptor_t desc);

static unsigned short pcnetReadCSR(unsigned char reg)
{
	outportw(io_base + REG_RAP, reg);
	return inportw(io_base + REG_RDP);
}
static void pcnetWriteCSR(unsigned char reg, unsigned short val)
{
	outportw(io_base + REG_RAP, reg);
	outportw(io_base + REG_RDP, val);
}
static unsigned short pcnetReadBCR(unsigned char reg)
{
	outportw(io_base + REG_RAP, reg);
	return inportw(io_base + REG_BDP);
}
void pcnetWriteBCR(unsigned char reg, unsigned short val)
{
	outportw(io_base + REG_RAP, reg);
	outportw(io_base + REG_BDP, val);
}

static void pcnet_setup_dma_map_buf(struct pcnet_softc *sc){
    unsigned long addr = 0;
    // 512 KiB
    alloc_pages(0, 128, (unsigned long*)&addr);

    if(!addr) {
        kernel_panic("pcnet");
    }
    memset((void*)addr, 0, 128*0x1000);

    sc->phymem = get_phy_addr(addr);
    sc->vmem = addr;

    if(sc->phymem > (0xFFFFFFFF - 0x80000 )) {
        kernel_panic("pcnet, the pcnet fest3 not support 64-bit memory physical address %x", sc->phymem);
    }


    sc->pcnet_block_vmem = sc->vmem;
    sc->pcnet_block_phymem = sc->phymem;

    sc->phymem += 8192;
    sc->vmem += 8192;

    sc->tx_buf = sc->phymem;
    sc->rx_buf = sc->phymem + 8192 + (PCNET_TX_BUF_NUM*8192);

    sc->tx_buf_v = sc->vmem;
    sc->rx_buf_v = sc->vmem + 8192 + (PCNET_TX_BUF_NUM*8192);
    
    sc->tx_buf_v += 8192;
    sc->rx_buf_v += 8192;

    sc->tx_buf_num = PCNET_TX_BUF_NUM;
    sc->rx_buf_num = PCNET_RX_BUF_NUM;

    sc->tx_cur = 0;
    sc->rx_cur = 0;

    for(int i=0; i < PCNET_TX_BUF_NUM; i++){
        sc->tx_desc[i] = (struct TxDesc *)(sc->vmem + i*sizeof(struct TxDesc));
        unsigned long addr = sc->tx_buf + 8192 + i*8192;
        sc->tx_desc[i]->buffer = (unsigned int)addr;
    }

    for(int i=0; i < PCNET_RX_BUF_NUM; i++){
        sc->rx_desc[i] = (struct RxDesc *)(sc->vmem + (8192 + PCNET_TX_BUF_NUM*8192) + i*sizeof(struct RxDesc));
        unsigned long addr = sc->rx_buf + 8192 + i*8192;
        sc->rx_desc[i]->buffer = (unsigned int)addr;
        unsigned short bcnt = (unsigned short)(-8192);
        bcnt &= 0x0fff;
        bcnt |= 0xf000;
        sc->rx_desc[i]->buf_length = bcnt;
        sc->rx_desc[i]->status = 0x8000;
    }
}


static void pcnet_get_hw_mac_address(struct pcnet_softc *sc, unsigned char *eaddr){

    unsigned int data = pcnetReadCSR(12);
    data |= pcnetReadCSR(12 + 1) << 16;
    eaddr[0] = ((data & 0x000000FF)>>0) & 0xFF;
    eaddr[1] = ((data & 0x0000FF00)>>8) & 0xFF;
    eaddr[2] = ((data & 0x00FF0000)>>16) & 0xFF;
    eaddr[3] = ((data & 0xFF000000)>>24) & 0xFF;
    data = pcnetReadCSR(12 + 2);
    eaddr[4] = ((data & 0x000000FF)>>0) & 0xFF;
    eaddr[5] = ((data & 0x0000FF00)>>8) & 0xFF;
}

void setup_pcnet( int bus, int slot, int function ){
    unsigned char mac_address[6];
    memset(&pcnet_softc,0, sizeof(struct pcnet_softc));
    struct pcnet_softc *sc = &pcnet_softc;

    printf("[PCNET] PCNET initialised bus=%x slot=%x function=%x \n",bus,slot,function);

    io_base = pci_read_config_dword(bus,slot,function,0x10);
    io_base &= ~1;
    printf("[PCNET] io base address: %x \n",io_base);

	int intr = pci_read_config_dword(bus,slot,function,0x3C) & 0x000000FF;
    int line = (pci_read_config_dword(bus,slot,function,0x3C) >> 8) & 0x000000FF;
    if(intr == 9) {
        // TODO improviso para VirtualBox
        // IRQ19 
        intr += 10;
    }

    printf("[PCNET] INTR %d LINE %d \n",intr, line);
    // Configurar IRQ Handler
    fnvetors_handler[intr] = &irq_pcnet;

    pcnet_pci(bus, slot, function);

    pcnet_setup_dma_map_buf(sc);
    //outportd(io_base + REG_RDP, 0);

    pcnet_get_hw_mac_address(sc, mac_address);
    printf("[PCNET] MAC %x:%x:%x:%x:%x:%x \n",mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);

    // Data Structure Initialization Block
    pcnet_block = (struct pcnet_block *)sc->pcnet_block_vmem;
   
	pcnet_block->mode = (3 << 28) | (5 << 20);
	pcnet_block->phys_addr1 = 0;
	memcpy(&pcnet_block->phys_addr0, mac_address, 6);
	pcnet_block->logical_addr_f0 = 0;
	pcnet_block->logical_addr_f1 = 0;
	pcnet_block->rx_buffer = sc->rx_buf;
	pcnet_block->tx_buffer = sc->tx_buf;

    // reset card
	//inportw(io_base + REG_RESET);

    // Switch pcnet to 32bit mode
	pcnetWriteBCR(20, 2);
    
    // Set reset autoselect bit
	int val = pcnetReadBCR(2) & ~2;
	val |= 2;
	pcnetWriteBCR(2, val);

	/* Enable auto negotiate, setup, disable fd */
	val = pcnetReadBCR(32) & ~0x98;
	val |= 0x20;
	pcnetWriteBCR(32, val);


	// Initialise
	unsigned int paddr = sc->pcnet_block_phymem;
	pcnetWriteCSR(1, paddr & 0xffff);
	pcnetWriteCSR(2, (paddr >> 16) & 0xffff);

    pcnetWriteCSR(4, 0x0915);
	pcnetWriteCSR(0, 0x0001);	// start

    // Wait for Init Done bit
    int i;
	for (i = 10000; i > 0; i--) {
		if (pcnetReadCSR(0) & 0x0100)
			break;
		//udelay (10);
	}
	if (i <= 0) {
		printf ("%s: timeout: controller init failed\n", "pcnet");
        while(1){}
		return;
	}

    pcnetWriteCSR(0, CSR_STATUS_INIT | CSR_STATUS_IENA | CSR_STATUS_STRT);

    register_ethernet_device((unsigned long)&pcnet_send_package,(unsigned long)&pcnet_recieve_package,mac_address);
}


static void irq_pcnet(){
    unsigned int status = pcnetReadCSR(0);
    printf("[PCNET] Interrupt detected %x\n", status );

    status &= ~CSR_STATUS_INTR;

    if( status & CSR_STATUS_RINT )
	{
		printf("[PCNET] Package recieved!\n");
        handler_ethernet_package_received(); 
	}
	
	// Tx Interrupt
	if( status & CSR_STATUS_TINT )
	{
		printf("[PCNET] Package send!\n");
	}

    if( status & CSR_STATUS_IDON )
	{
		printf("CSR15 reads as 0x%x\n", pcnetReadCSR(15));
	}

    // set interrupt as handled
    pcnetWriteCSR(0, status);
    // don't forget to send EOI
}

int pcnet_send_package(ethernet_package_descriptor_t desc){

    struct pcnet_softc *sc = (struct pcnet_softc *)&pcnet_softc;

    unsigned long long dest = sc->tx_buf_v + (8192*sc->tx_cur);

    if(desc.buffersize > 8192){
        printf("The package is too big\n");
        return 2;
    }

    memcpy((char*)dest, desc.buf, desc.buffersize);

    int pkt_len = desc.buffersize;
    sc->tx_desc[sc->tx_cur]->length = (-pkt_len);
    sc->tx_desc[sc->tx_cur]->misc = 0x00000000;
    sc->tx_desc[sc->tx_cur]->status = 0x8300;
    pcnetWriteCSR(0, pcnetReadCSR(0)| 0x0008);

    // next ptr
    sc->tx_cur = (sc->tx_cur + 1) % sc->tx_buf_num;
    return 0;
}

ethernet_package_descriptor_t *pcnet_recieve_package(){

    struct pcnet_softc *sc = (struct pcnet_softc *)&pcnet_softc;

    ethernet_package_descriptor_t *desc = (ethernet_package_descriptor_t *)&packege_desc_buffer;
    ethernet_package_descriptor_t *first_desc = desc; 

    desc->flag = -1;
    desc->count = 0;
    desc->buffersize = 0;
    desc->buf = (void*)0;
   
    while(!(sc->rx_desc[sc->rx_cur]->status & 0x8000) ){

        sc->rx_desc[sc->rx_cur]->status |= 0x8000;

        unsigned long long addr = sc->rx_buf_v + (8192*sc->rx_cur);

        unsigned short len = (sc->rx_desc[sc->rx_cur]->msg_length & 0xfff) - 4;
        sc->rx_cur = (sc->rx_cur + 1) % sc->rx_buf_num;
        desc->buffersize = len;
        desc->buf = (void*) addr;

        desc->flag = 0;
        first_desc->count++;
        desc++;

       
    }    
    
    return first_desc;
}

static void pcnet_pci(int bus, int slot, int function){

    unsigned int command = 0;
    command = pci_read_config_dword(bus,slot,function, 4);
    command |= (0x5);
    pci_write_config_dword(bus,slot,function, 4, command);
    command = pci_read_config_dword(bus,slot,function, 4);

    // Enable o PCI Busmastering DMA
    if(!(command&0x04))
    {
        printf("[PCNET] Busmastering was not enabled\n");
    }

}
