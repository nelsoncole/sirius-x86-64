#include <io.h>
#include <pci.h>
#include "hda.h"
#include <mm.h>
#include <string.h>
#include <sleep.h>
#include <stdio.h>

#include "wav.h"

unsigned int HDBARL, HDBARU;
unsigned int HDA_MEM_SIZE;
unsigned long HDA_MEM;
HDA_T *hda_base;

unsigned int corb_pointer;

unsigned long CORB_MEMORY;
unsigned long RIRB_MEMORY;
unsigned long HDA_STREAM_MEMORY;

unsigned int hda_widget[32];

unsigned int output_node;
unsigned int input_node;
unsigned int hda_max_volume;
unsigned int  beep_node;

void hda_wait() {
	sti();
	sleep(10);
	cli();
}

void hda_outb(unsigned int offset, unsigned char val) {
	unsigned char *mem = (unsigned char *) (HDA_MEM + offset);
	mem[0]=val;
}

void hda_outw(unsigned int offset, unsigned short val) {
	unsigned short *mem = (unsigned short *) (HDA_MEM + offset);
	mem[0]=val;
}

void hda_outl(unsigned int offset, unsigned int val) {
	unsigned int *mem = (unsigned int *) (HDA_MEM + offset);
	mem[0]=val;
}

unsigned char hda_inb(unsigned int offset) {
	unsigned char *mem = (unsigned char *) ( HDA_MEM + offset);
	return (mem[0]);
}

unsigned short hda_inw(unsigned int offset) {
	unsigned short *mem = (unsigned short *) ( HDA_MEM + offset);
	return (mem[0]);
}

unsigned int hda_inl(unsigned int offset) {
	unsigned int *mem = (unsigned int *) ( HDA_MEM + offset);
	return (mem[0]);
}

void corb_write_command(unsigned int cad, unsigned int nid, unsigned int verb, unsigned int cmd) {

	unsigned int *corb = (unsigned int *) CORB_MEMORY;	
	
	corb[corb_pointer] = 0;
	corb[corb_pointer] |= cad << 28;
	corb[corb_pointer] |= nid << 20;
	corb[corb_pointer] |= verb << 8;
	corb[corb_pointer] |= cmd;
	
	corb_pointer ++;
}

void hda_reset() {
	hda_outw(0x8, 0);
	hda_wait();
	while( (hda_inb(0x8)&1) );
	
	hda_outw(0x8, 0x101);
	hda_wait();
	while( !(hda_inb(0x8)&1) );

}

HDA_RESPONSE *hda_response();
int hda_send_verb(int cad, int nid, int verb, int cmd);
void hda_set_output_node(int node);
void hda_play_sound();
void hda_volume(int volume, int node);
unsigned int hda_ic_verbe(int cad, int nid, int verb, int cmd);

static int hda_pci_configuration_space(int bus,int dev,int fun);
int setup_hda(void) {

	unsigned int data = pci_scan_bcc_scc(4,3);
	HDBARL = HDBARU = 0;
	corb_pointer = 0;
	output_node = input_node = 0;
	hda_max_volume = 0;
	 beep_node =0;
	
	if(data == -1) {
		printf("PCI PANIC: Intel High Definition Audio (HDA) Controller not found!\n");
		return -1;
	}	 
	
	printf("PCI Intel High Definition Audio Controller initialize\n");

	data = hda_pci_configuration_space(data  >>  24 &0xff,data  >> 16  &0xff,data &0xffff);

	if(data) {
	
		printf("PCI PANIC: Intel High Definition Audio (HDA) Controller error!\n");
		return -1;
	}

	HDA_MEM_SIZE = 0x20000; // 128 Kilobytes
    mm_mp( HDBARL , &HDA_MEM, HDA_MEM_SIZE,0);
    hda_base = (HDA_T*) HDA_MEM;

	printf("Intel High Definition Audio Controller version %d.%d\n",hda_base->vmaj,hda_base->vmin);

	alloc_pages(0, 2, (unsigned long *)&CORB_MEMORY); // 8 KiB
	memset( (void *) CORB_MEMORY, 0, 0x2000);
	RIRB_MEMORY = CORB_MEMORY + 0x1000;
	
	alloc_pages(0, 32, (unsigned long *)&HDA_STREAM_MEMORY); //128 KiB
	memset( (void *) HDA_STREAM_MEMORY, 0, 0x20000);

	/*unsigned int *corb = (unsigned int *) CORB_MEMORY;
	unsigned int *rirb = (unsigned int *) RIRB_MEMORY;*/
	
	hda_reset();
	
	// HDA disable interrupt
	hda_outl(0x20, 0x0 );
	// Off corb e rirb
	hda_outb(0x4C, 0x0 ); // CORB control
	hda_outb(0x5C, 0x0 ); // RIRB control
	hda_outl(0x70, 0x0 ); // DMA POS control
	hda_outl(0x74, 0x0 ); // DMA POS control
	
	hda_outl(0x60, 0 ); // Immediate Command Output Interface
	hda_outl(0x64, 0 ); // Immediate Command Input Interface
	hda_outw(0x68, 0 ); // Immediate Command Status
	
	// HDA Input Stream Turn Off
	hda_outl(0x80 , 0x200000 );
	// HDA Output Stream Turn Off
	hda_outl(0x100, 0x140000 );
	
	unsigned long long phy = get_phy_addr(HDA_STREAM_MEMORY);
	
	// HDA Output Stream Set Buffer
	hda_outl(0x118, phy);
	hda_outl(0x11C, phy >> 32);
	hda_outw(0x124, 1 ); // Link Position in Buffer
	
	phy += 0x10000;
	
	// HDA Input Stream Set Buffer
	hda_outl(0x98, phy);
	hda_outl(0x9C, phy >> 32);
	
	// HDA Set Stream Synchronization
	hda_outl(0x34, 0);
	//hda_outl(0x38, 0);
	
	
	hda_outb(0x48, 0);
	hda_outb(0x4C, 0);
	while( (hda_inb(0x4C)&2) ) hda_wait();
	
	hda_outb(0x5C, 0);
	while( (hda_inb(0x5C)&2) ) hda_wait();
	
	phy = get_phy_addr(CORB_MEMORY);
	hda_outl(0x40, phy);  //CORB Lower Base Address
	hda_outl(0x44, phy >> 32);  //CORB Upper Base Address
	
	phy = get_phy_addr(RIRB_MEMORY); 
	hda_outl(0x50, phy);  //RIRB Lower Base Address 
	hda_outl(0x54, phy >> 32);  //RIRB Upper Base Address

	// NULL
	for(int i=0; i < 10; i++) {
		corb_write_command(0, 0, 0xf00, 0);
	}
	
	hda_outb(0x48, 10); 
	hda_outb(0x5A, 10);
	
	hda_outw(0x4A, 0x8000); //reset corb
	while( (hda_inw(0x4A) & 0x8000) != 0x8000) hda_wait();
	
	hda_outw(0x4A, 0x0000); //reset corb
	while( (hda_inw(0x4A) & 0x8000) != 0x0000) hda_wait();
	
	hda_outw(0x58, 0x8000);

	hda_outb(0x5C, (hda_inb(0x5C) & 0xFA) | 2);
	hda_outb(0x4C, (hda_inb(0x4C) & 0xFE) | 2);
	
	while(hda_inb(0x58)==0) { 
		
		hda_inw(0x4A); 
		hda_wait(); 
		
	}
	
	printf("\nGCAP:%x OUTPAY:%x INPAY:%x GCTL:%x WAKEEN:%x STATESTS %x GSTATUS:%x ",
	hda_base->gcap, hda_base->outpay, hda_base->inpay, hda_base->gctl, hda_base->wakeen, hda_base->statests, hda_base->gsts);
	
	printf("\nCORBWP:%x CORBRP:%x CORBCTRL:%x CORBSTATUS:%x CORBSIZE:%x\n",
	hda_base->corbwp,hda_base->corbrp,hda_base->corbctl,hda_base->corbsts,hda_base->corbsize);
	
	
	
	int widget_len = 0;
	memset(hda_widget, 0, sizeof(unsigned int)*32);
	HDA_RESPONSE *rp;
	
	struct _HDA_ROOT_NODE root;
	struct _HDA_AFG_NODE afg;
	struct _HDA_OUTPUT_NODE out;
	
	rp = hda_response();
	
	
	
	printf("Root Node: ");
	if( hda_send_verb(0, 0, 0xf00, 4) )
	{
			printf("error:\n");
	}
	
	printf("%x\n", rp->response);
	
	root.start = rp->response >> 16 &0xf;
	root.num = rp->response &0xff;
	
	printf("Start Node %d, Parametro 5:  ", root.start);
	if( hda_send_verb(0, root.start, 0xf00, 5) )
	{
			printf("error:\n");
	}
	
	printf("%x\n", rp->response);
	
	if( hda_send_verb(0, root.start, 0xf00, 4) )
	{
			printf("error:\n");
	}
	
	afg.start = rp->response >> 16 &0xf;
	afg.num = rp->response &0xff;
	
	printf("AFG: %d %d\n",afg.start,afg.num);
	
	
	
	int output = 0, input = 0;
	
	for(int i = 1; i < (afg.num+afg.start); i++){
		if( hda_send_verb(0,  i, 0xf00, 9) )
		{
			printf("CORB/RIRB error:\n");
		}
		
		rp = hda_response();
		
		hda_widget[widget_len ++] = rp->response;
		
		if(rp->response == -1 || rp->response == 0) continue;
			
		if((rp->response >> 20 &0xf) == 0 && output == 0) {
			output = 1;
			output_node = i;
			out.start =  i;
			printf("Output stream node %d\n", out.start);
			
		}else if((rp->response >> 20 &0xf) == 1 && input == 0){
			input = 1;
			input_node = i; 
			printf("Input stream node %d\n", input_node);
		}else if((rp->response >> 20 &0xf) == 7){
			beep_node = i; 
			printf("Beep Widget node %d\n", beep_node);
		}
		
	}
	
	
	printf("\nNumero de AFG: %d\n",widget_len);
	
	for(int i=0; i < widget_len; i++) {
		printf("%x ",hda_widget[i] );
	}
	putchar('\n');
	
	printf("HDA OUTPUT NODE: %d\n",out.start);
	hda_send_verb(0, out.start, 0x705, 0);
	for(int i=0; i < 100; i++)hda_wait();
	hda_send_verb(0, out.start, 0xf05, 0);
	printf(": %x\n", rp->response);
	
	hda_send_verb(0, out.start, 0xf00, 0x12);
	printf("Detalhes do amplificador de saida: %x\n", rp->response);
	
	hda_send_verb(0,beep_node,0x70a,0xff);
	hda_send_verb(0,beep_node,0xf0a,0);
	printf("Beep: %x\n",rp->response);

	
	
	for(int i=0; i < 4; i++)
	{
		hda_volume(100, out.start+i);
		hda_send_verb(0, out.start +i, 0xb00, 0); // set volume
		printf(": VOLUME %x\n", rp->response);
	
		hda_send_verb(0, out.start + i, 0xf00, 0x12);
		printf("Detalhes do amplificador de saida: %x\n", rp->response);
	}
	
	
	//for(output_node= afg.start; output_node < (afg.start+afg.num); output_node++){
		hda_play_sound();
		hda_set_output_node(out.start);
		
		
	//}
	return 0;
}


static int hda_pci_configuration_space(int bus,int dev,int fun)
{	

	unsigned int data;

	// STATUS CMD Enable Memory mapped and Bus Master
	data = read_pci_config_addr( bus, dev, fun, 0x04);
	write_pci_config_addr( bus , dev, fun, 0x04, data | 0x6 );
	
 	
 	// BAR0-1
	data = read_pci_config_addr( bus, dev, fun, 0x10);
	HDBARL = data &0xfffffff0;
	
	data = read_pci_config_addr( bus, dev, fun, 0x14);
	HDBARU = data;
	
	//TCSEL
	data = read_pci_config_addr( bus, dev, fun, 0x44);
	write_pci_config_addr( bus , dev, fun, 0x44, data & 0xffffff00 );
	
	
	return 0;
}

HDA_RESPONSE *hda_response()
{
	HDA_RESPONSE *resp = (HDA_RESPONSE*) (RIRB_MEMORY + 8);
	return resp;
}

int hda_send_verb(int cad, int nid, int verb, int cmd)
{
	corb_pointer = 0;
	
	unsigned long long *resp = (unsigned long long*) (RIRB_MEMORY);
	
	hda_outb(0x48, 0);
	hda_outb(0x4C, 0);
	while( (hda_inb(0x4C)&2) ) hda_wait();
	
	hda_outb(0x5A, 0);
	hda_outb(0x5C, 0);
	while( (hda_inb(0x5C)&2) ) hda_wait();

	corb_write_command(0, 0, 0, 0); // NULL
	corb_write_command(cad, nid, verb, cmd);
	corb_write_command(0, 0, 0, 0); // NULL
	
	hda_outb(0x48, 3); 
	hda_outb(0x5A, 3);
	
	hda_outw(0x4A, 0x8000); //reset corb
	while( (hda_inw(0x4A) & 0x8000) != 0x8000) 
		hda_wait();
		
	
	
	hda_outw(0x4A, 0x0000); //reset corb
	while( (hda_inw(0x4A) & 0x8000) != 0x0000) 
		hda_wait();
	
	hda_outw(0x58, 0x8000);

	hda_outb(0x5C, (hda_inb(0x5C) & 0xFA) | 2);
	hda_outb(0x4C, (hda_inb(0x4C) & 0xFE) | 2);
	
	int spin = 100;
	while(hda_inb(0x58) == 0) { 
		
		hda_inw(0x4A); 
		hda_wait(); 
		
		if(!spin--) return -1;
	}

	return 0;
}


unsigned int hda_ic_verbe(int cad, int nid, int verb, int cmd)
{
	unsigned int data = 0;
	data |= cad << 28;
	data |= nid << 20;
	data |= verb << 8;
	data |= cmd;
	
	// immediate command
	hda_outw(0x68, 0x2);
	
	// immediate output
	hda_outl(0x60, data);
	
	hda_outw(0x68, 0x1);
	
	// wait
	int spin = 0;
	for( spin =0; spin < 100; spin ++) {
	
		if( (hda_inb(0x68) &0x3) == 0x2) break;
		hda_wait();
	}
	
	if(spin > 100) return -1;
	
	// immediate input
	unsigned int resp = hda_inl(0x64);
	hda_outw(0x68, 0x2);
	return resp;

}


void hda_set_output_node(int node)
{
	printf("HDA set output node\n");
	HDA_RESPONSE *rp;
	// Get amplifier info
	if( hda_send_verb(0, node, 0xf00, 0x12) ) {
		printf("CORB error: Get amplifier info\n");
	}else {
		rp = hda_response();
		
		hda_max_volume = 0x40;//(unsigned int) rp->response &0xff;
		printf("Get amplifier info: %x, %d\n", rp->response, hda_max_volume);
		
		printf("Set stream: stream 1 chanel 1\n");
		if( hda_send_verb(0, node, 0x706, 0x10) ) {
			printf("CORB error: Set stream\n");
		}else {
			rp = hda_response();
			printf("%x\n",rp->response );
			
			printf("Set volume\n");
			if( hda_send_verb(0, node, 0x300, hda_max_volume | 0xB000 ) ) {
				printf("CORB error: Set volume\n");
			}else {
				rp = hda_response();
				printf("%x\n",rp->response );
			
			}
			
		}
		
	}

}


void hda_play_sound()
{
	// set output buffer
	unsigned int *output = (unsigned int *) HDA_STREAM_MEMORY;
	
	unsigned long long a = 0;//0x10000;//0x200000;
	WAV_HEADER *wav = (WAV_HEADER*)a;
	int size = 512;
	
	/*char *c = (char*)a;
	
	memset((void*)a, 0, size);
	
	*c++ = 0x7f;
	*c++ = 0x45;
	*c++ = 0x4c;
	*c++ = 0x46;
	*c++ = 0x1;
	*c++ = 0x1;
	*c++ = 0x1;*/
	
	/*FILE *fp = fopen("beep.wav","r+b");
	
	if(fp != 0) {
	
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		rewind(fp);
	
		fread ((char*)a, 1, size, fp);
		
		fclose(fp);

		
		printf("WAV HEADER:\n");
		printf("riff_tag: %c%c%c%c\n",wav->riff_tag[0],wav->riff_tag[1],wav->riff_tag[2],wav->riff_tag[3]);
		printf("riff_length: %d\n",wav->riff_length);
		printf("wave_tag: %c%c%c%c\n",wav->wave_tag[0],wav->wave_tag[1],wav->wave_tag[2],wav->wave_tag[3]);
		printf("fmt_tag: %c%c%c%c\n",wav->fmt_tag[0],wav->fmt_tag[1],wav->fmt_tag[2],wav->fmt_tag[3]);
		printf("fmt_length: %d\n",wav->fmt_length);
		printf("audio_format: %d\n",wav->audio_format);
		printf("num_channels: %d\n",wav->num_channels);
		printf("sample_rate: %d\n",wav->sample_rate);
		printf("byte_rate: %d\n",wav->byte_rate);
		printf("block_align: %d\n",wav->block_align);
		printf("bits_per_sample: %d\n",wav->bits_per_sample);
		printf("data_tag: %c%c%c%c\n",wav->data_tag[0],wav->data_tag[1],wav->data_tag[2],wav->data_tag[3]);
		printf("data_length: %d\n",wav->data_length);
		
		a += sizeof(WAV_HEADER);
		
		size -= sizeof(WAV_HEADER);
	}
	else
		printf("Read error\n");*/
	
	
	output[0] = a;
	output[1] = 0;
	output[2] = size;
	output[3] = 0;
	/*
	output[4] = a;
	output[5] = 0;
	output[6] =  size;
	output[7] = 0;*/
	
	
	hda_outl(0x108, size);  // Cyclic Buffer Length
	hda_outw(0x112, 0x4300); // output format (48 kHz, 44.1 kHz)
	
	hda_outw(0x10C, 1);
	
	hda_outl(0x70, 0 | 1);
	hda_outl(0x74, 0);
	
	hda_outl(0x100, 0x140002 );
	
	hda_outl(0x34, 0x100);
	
	for(int i=0; i < 100; i++)
	hda_wait();
	
	//hda_outl(0x100, 0x140000 );
	hda_wait();
	
	printf("%x %x\n",hda_inl(0x70), hda_inl(0x74));
	
}



void hda_volume(int volume, int node)
{
	hda_max_volume = 0x4A;
	
	int a = hda_max_volume * volume;
	
	a = a / 100;
	
	a |= 0xB000;
	
	hda_send_verb(0, node, 0x300, a)	;

}














