/*
 * Credits:
 *      Sander de Regt 
 *
 */

#include <ethernet.h>
#include <pci.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include <io.h>

#include <socket.h>

#include "udp.h"
#include "tcp.h"
#include "ipv4.h"
#include "arp.h"
#include "dhcp.h"

unsigned int dst_ip = 0;
unsigned int src_ip = 0;
extern unsigned int ipv4_count;
ethernet_package_descriptor_t packege_desc_buffer[32];

ethernet_device_t default_ethernet_device;
unsigned long package_recieved_ack = 0;
ethernet_package_descriptor_t default_ethernet_package_descriptor;

unsigned char our_ip[SIZE_OF_IP];
unsigned char router_ip[SIZE_OF_IP];
unsigned char dns_ip[SIZE_OF_IP];
unsigned char dhcp_ip[SIZE_OF_IP];

ethernet_device_t get_default_ethernet_device(){
    return default_ethernet_device;
}

void ethernet_set_link_status(unsigned long a){
    default_ethernet_device.is_online = a;
}

void ethernet_set_interrupt_status(unsigned long a){
    default_ethernet_device.is_interrupt = a;
}


void register_ethernet_device(unsigned long send_package,unsigned long receive_package,unsigned char mac[SIZE_OF_MAC])
{
    default_ethernet_device.receive_package = receive_package;
    default_ethernet_device.send_package = send_package;
    
    default_ethernet_device.is_enabled = 1;

    for(int i = 0 ; i < SIZE_OF_MAC ; i++){
        default_ethernet_device.mac[i] = mac[i];
    }
}

ethernet_package_descriptor_t *get_ethernet_package(){
    ethernet_device_t ed = get_default_ethernet_device();
    if(!ed.is_enabled)return 0;

    ethernet_package_descriptor_t *(*get_package)() = (void*)default_ethernet_device.receive_package;
    return get_package();
}

int send_ethernet_package(const void *buf, size_t size){
    ethernet_device_t ed = get_default_ethernet_device();
    if(!ed.is_enabled)return 0;

    int (*send_package)(ethernet_package_descriptor_t) = (void*)default_ethernet_device.send_package;

    ethernet_package_descriptor_t desc;
    desc.buffersize = size;
    desc.buf = (void*) buf;

    return send_package(desc);
}

void handler_ethernet_package_received(){

    if(/*!default_ethernet_device.is_interrupt ||*/ !default_ethernet_device.is_online) return;

    ether_header_t *eh;
    arp_header_t *arp;
    ipv4_header_t *ipv4;
    ipv4_header_t *ipv4_cp;
    udp_header_t *udp;
    tcp_header_t *tcp;

    char *data, *end;
    int len;

    unsigned char mac[SIZE_OF_MAC] = {0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char ip[SIZE_OF_IP] = {0,0,0,0};
    unsigned char target_ip[SIZE_OF_IP] = {0,0,0,0};

    package_recieved_ack = 0;

    ethernet_package_descriptor_t *prd;
    prd = get_ethernet_package();
    
    if(!prd) return;

    int count = (int) prd->count;

loop:

    if(prd->buf == 0 || prd->buffersize == 0 || prd->flag) goto next;

    eh = (ether_header_t*) prd->buf;
 
    switch( htons(eh->type) ){
        case ET_ARP:
            arp = (arp_header_t*) (prd->buf + sizeof(ether_header_t) );
            fillMac(mac, arp->source_mac);
            fillIP(ip, arp->source_ip);
            fillIP(target_ip, arp->dest_ip);

            switch( htons(arp->operation) ){
                case ARP_OPC_REQUEST:
                    printf("ARP REQUEST\n");
                    arp_save_address( ip, mac);
                    arp_replay(ip, mac);

                    //repaly packet
                    /*data = (char*)ipv4_cache;
                    ipv4_cp = (ipv4_header_t*)(ipv4_cache + sizeof(ether_header_t));
                    if((ipv4_cp->dst == *(unsigned int*)target_ip) && (ipv4_cp->checksum != 0)){
                        send_ethernet_package(data, htons(ipv4_cp->len) + sizeof(ether_header_t));
                        ipv4_cp->checksum = 0;
                    }*/
                    break;
                case ARP_OPC_REPLY:
                    printf("ARP REPLAY\n");
                    arp_save_address( ip, mac);
                    break;
                default:
                    printf("ARP UNKNOWN %x\n", htons(arp->operation) );
                    break;
            }   
            break;
        case ET_IPV4:
            ipv4 = (ipv4_header_t*) (prd->buf + sizeof(ether_header_t) );
            switch( ipv4->protocol){
                case IP_PROTOCOL_ICMP:
                    printf("IPv4 ICMP\n");
                    break;
                case IP_PROTOCOL_TCP:
                    printf("IPv4 TCP\n");
                    tcp = (tcp_header_t*) ((unsigned long)(prd->buf + sizeof(ipv4_header_t) + sizeof(ether_header_t)));
                    data = (char*) tcp;
                    end = (char*) tcp;
                    data += (tcp->off >>4)*4;
                    end += htons(ipv4->len) - sizeof(ipv4_header_t);
                    len =  end - data;
                    socket_server_receive(0, IP_PROTOCOL_TCP, ipv4->src, ipv4->dst, tcp->src_port,
                    tcp->dst_port, data, len, tcp->seq, tcp->ack, tcp->flags);
                    break;
                case IP_PROTOCOL_UDP:
                    printf("IPv4 UDP\n");
                    udp = (udp_header_t*) ((unsigned long)(prd->buf + sizeof(ipv4_header_t) + sizeof(ether_header_t)));
                    data = (char*) udp;
                    end = (char*) udp;
                    data += sizeof(udp_header_t);
                    end += htons(ipv4->len) - sizeof(ipv4_header_t);
                    len =  end - data;
                    socket_server_receive(0,IP_PROTOCOL_UDP, ipv4->src, ipv4->dst, udp->src_port, udp->dst_port, data, len, 0, 0, 0);
                  
                    break;
                default:
                    printf("IPv4 UNKNOWN %x\n", ipv4->protocol );
                    break;
            }

            break;
        case ET_IPV6:
            printf("Packege Receive: ethernet type = %x [IPV6]\n", htons(eh->type) );
            break;
        default:
            printf("Packege Receive: ethernet type =%x\n", htons(eh->type) );
            break;
    }

next:

    prd++;
    count--;

    if(count > 0) goto loop;
}

// RealTek PCI vendor ID
#define	REALTEK_VENDORID 0x10EC
// D-Link vendor ID
#define DLINK_VENDORID  0x1186

int int_ethernet_device()
{
    int bus, slot, function;
    unsigned short device, vendor;
    unsigned int data = pci_scan_class_subclass(2, 0);

    memset(&default_ethernet_device, 0, sizeof(ethernet_device_t));

    if(data == -1) {
		printf("PCI PANIC: Network Controller not found!\n");
		return -1;
	}

    bus = data  >>  24 &0xff;
    slot = data  >> 16  &0xff;
    function = data &0xffff;
    
    data = pci_read_config_dword(bus,slot,function,0);

    device = (data >> 16) &0xFFFF;
    vendor = data &0xFFFF;
    
    
    if( vendor == 0x8086 && (device == 0x100e || device == 0x153A || device == 0x10EA) ) {

        init_e1000( bus, slot, function );

    } else if(vendor == REALTEK_VENDORID \
    && (device == 0x8169 || device == 0x8168 || device == 0x8161 || device == 0x8136 || device == 0x4300)){
        setup_realtek( bus, slot, function );
    } else {
        printf("Unknown ethernet device\n");
        printf("Other Network device id %x vendor id %x\n", device, vendor);
    }

    return 0;
}

void fillMac(unsigned char* to,unsigned char* from){
    for(int i = 0 ; i < SIZE_OF_MAC ; i++){
        to[i] = from[i];
    }
}

void fillIP(unsigned char* to,unsigned char* from){
    for(int i = 0 ; i < SIZE_OF_IP ; i++){
        to[i] = from[i];
    }
}


void initialise_ethernet(){

    ipv4_cache = (unsigned char *)malloc(0x10000); // 64KiB
    dhcp_header_t *dhcp = (dhcp_header_t *)malloc(sizeof(dhcp_header_t)); 
    package_recieved_ack = 1;
    ipv4_count  = 0;
    memset(ipv4_cache, 0, 0x10000);

    printf("[ETH] Ethernet module reached!\n");
    ethernet_device_t ed = get_default_ethernet_device();
    if(ed.is_enabled){

        memset(dhcp, 0, sizeof(dhcp_header_t));
        dhcp_send(our_ip, dhcp_ip, DHCP_DISCOVER);
        if( udp_receive(dhcp, sizeof(dhcp_header_t), 68) ){
            if(htonl(dhcp->magic_cookie) != 0x63825363) {
                printf("No DHCP\n");
                free(ipv4_cache);
                goto end;
            }
            printf("%s\n", string_dhcp_message[dhcp->options[2]]);
            dhcp_parse_options(dhcp);
        }else {
        
            printf("timeout\n");
            free(ipv4_cache);
            goto end;
        }

        printf("Your    IP %d.%d.%d.%d \n",our_ip[0],our_ip[1],our_ip[2],our_ip[3]);
        printf("Gateway IP %d.%d.%d.%d \n",router_ip[0],router_ip[1],router_ip[2],router_ip[3]);
        printf("DNS     IP %d.%d.%d.%d \n",dns_ip[0],dns_ip[1],dns_ip[2],dns_ip[3]);
        printf("DHCP    IP %d.%d.%d.%d \n",dhcp_ip[0],dhcp_ip[1],dhcp_ip[2],dhcp_ip[3]);    

        memset(dhcp, 0, sizeof(dhcp_header_t));
        dhcp_send(our_ip, dhcp_ip, DHCP_REQUEST);
        if( udp_receive(dhcp, sizeof(dhcp_header_t), 68) ){
            if(htonl(dhcp->magic_cookie) != 0x63825363) {
                printf("No DHCP\n");
                free(ipv4_cache);
                goto end;
            }

            if(dhcp->options[2] != DHCP_ACK){
                printf("%s\n", string_dhcp_message[dhcp->options[2]]);
                free(ipv4_cache);
                goto end;
            }

            printf("%s\n", string_dhcp_message[dhcp->options[2]]);
        }

        fillIP((unsigned char*)& default_ethernet_device.client_ip,(unsigned char*)&our_ip);
        fillIP((unsigned char*)&default_ethernet_device.server_ip,(unsigned char*)&dhcp_ip);

        ethernet_set_link_status(1);
    }else {
            printf("Internet device not found!\n");
            free(ipv4_cache);
            goto end;
    }

    // ARP CACHE
    init_arp();

    unsigned char ip[SIZE_OF_IP] =  {192,168,43,1};
    fillIP((unsigned char*)&src_ip, our_ip);
    fillIP((unsigned char*)&dst_ip, ip);

end:  
    free(dhcp);
    return;   
    //while(1){}
}
