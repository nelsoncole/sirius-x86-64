#include <ethernet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm.h>
#include <kernel.h>

#include "udp.h"

#define VMNET_RX_BUF_NUM		256		// Rx buffer number
#define VMNET_TX_BUF_NUM		(VMNET_RX_BUF_NUM)	// Tx buffer number

struct RxDesc
{
    unsigned long buffer;
	unsigned short length;
	unsigned short status;
}__attribute__((packed));

struct vm_inet_header
{
    unsigned int src;
    unsigned int dst;
    unsigned short len;
    unsigned char  protocol;
    unsigned char flags;

}__attribute__((packed));


struct RxDesc *rx_desc[VMNET_RX_BUF_NUM];

static ethernet_package_descriptor_t *vmnet_recieve_package();

static int rx_cur;
static int tx_cur;
static void vmnet_map_buf(){
    unsigned long addr = 0;
    // 2048 + 8 KiB
    alloc_pages(0, 514, (unsigned long*)&addr);

    if(!addr) {
        kernel_panic("vmnet");
    }
    memset((void*)addr, 0, 514*0x1000);

    unsigned long v = addr;
    addr += 8192;
    for(int i=0; i < VMNET_RX_BUF_NUM; i++){
        rx_desc[i] =(struct RxDesc *)(v + i*sizeof(struct RxDesc));
        rx_desc[i]->buffer = addr;
        addr += 8192;
        rx_desc[i]->status = 0x8000;
    }

    rx_cur = 0;
    tx_cur = 0;
}


void setup_vmnet(){
    vmnet_map_buf();

    local_ip[0] = 127;
    local_ip[1] = 0;
    local_ip[2] = 0;
    local_ip[3] = 1;
}

void handler_vmnet(){
    unsigned int status = 0;
    printf("[VMNET] Interrupt detected %x\n", status );
    int len;
    char *data, *end;
    udp_header_t *udp;
    ethernet_package_descriptor_t *prd;
    prd = vmnet_recieve_package();
    
    if(!prd) return;

    int count = (int) prd->count;

    for(int i=0; i < count; i++){
        if(prd->buf == 0 || prd->buffersize == 0 || prd->flag){
            prd++;
            
            continue;
        }
        
        struct vm_inet_header *hdr =(struct vm_inet_header*)prd->buf;
        switch(hdr->protocol){
            case IP_PROTOCOL_TCP:
                printf("VM NET IPv4 TCP\n");
                break;
            case IP_PROTOCOL_UDP:
                printf("VM NET IPv4 UDP\n");
                udp = (udp_header_t*) ((unsigned long)(prd->buf +sizeof(struct vm_inet_header)));
                data = (char*) udp;
                end = (char*) udp;
                data += sizeof(udp_header_t);
                end += htons(hdr->len) - sizeof(struct vm_inet_header);
                len =  end - data;
                socket_server_receive(0,IP_PROTOCOL_UDP, hdr->src, hdr->dst, udp->src_port, udp->dst_port, data, len, 0, 0, 0);
                break;
            default:
                printf("VM NET  IPv4 NULL\n");
                break;
        }

        prd++;

    }

}


int vmnet_send_package(ethernet_package_descriptor_t desc){

    unsigned long long dest = rx_desc[tx_cur]->buffer;

    if(desc.buffersize > 8192){
        printf("The package is too big\n");
        return 2;
    }

    memcpy((char*)dest, desc.buf, desc.buffersize);

    int pkt_len = desc.buffersize;
    rx_desc[tx_cur]->length = pkt_len;
    rx_desc[tx_cur]->status = 0;

    tx_cur = (tx_cur + 1) % VMNET_TX_BUF_NUM;
    // gerar interrupção
    asm("int $0xA0");

    return 0;
}

ethernet_package_descriptor_t *vmnet_recieve_package(){

    ethernet_package_descriptor_t *desc = (ethernet_package_descriptor_t *)&packege_desc_buffer;
    ethernet_package_descriptor_t *first_desc = desc; 

    desc->flag = -1;
    desc->count = 0;
    desc->buffersize = 0;
    desc->buf = (void*)0;
   
    while(!(rx_desc[rx_cur]->status & 0x8000) ){

        rx_desc[rx_cur]->status |= 0x8000;

        unsigned long long addr = rx_desc[rx_cur]->buffer;
        
        unsigned short len = rx_desc[rx_cur]->length;
        rx_cur = (rx_cur + 1) % VMNET_RX_BUF_NUM;

        desc->buffersize = len;
        desc->buf = (void*) addr;

        desc->flag = 0;
        first_desc->count++;
        desc++;

       
    }    
    
    return first_desc;
}

int vmipv4_send(void *buf, unsigned char protocol, unsigned int src_address, unsigned int dst_address, unsigned length){
    struct vm_inet_header *hdr = (struct vm_inet_header*)buf;
    unsigned len = sizeof(struct vm_inet_header) + length;
    
    hdr->src = src_address;
    hdr->dst = dst_address;
    hdr->protocol = protocol;
    hdr->flags = 0;
    hdr->len = htons(len);

    ethernet_package_descriptor_t desc;
    desc.buffersize = len;
    desc.buf = (void*) hdr;

    // Hardware
    if(vmnet_send_package( desc) ) 
        return 1;

    return 0;
}

int vmnet_udp_send(unsigned int src_ip, unsigned int dst_ip,
unsigned short src_port, unsigned short dst_port, const void *data, size_t length){

    unsigned len = sizeof(struct vm_inet_header) + sizeof(udp_header_t) + length;
    unsigned char *buf = (unsigned char*) malloc( len );
    udp_header_t *udp = (udp_header_t*) (buf + sizeof(struct vm_inet_header));

    udp->length = htons(sizeof(udp_header_t) + length);
    udp->dst_port = htons(dst_port);
    udp->src_port = htons(src_port);
    udp->checksum = 0;

    // data
    if(data != 0 || length > 0 )
        memcpy((unsigned char*)udp + sizeof(udp_header_t), data, length);
    // 

    len = sizeof(udp_header_t) + length;
    vmipv4_send(buf, IP_PROTOCOL_UDP, src_ip, dst_ip, len);

    free(buf);
 
    return length;

}
