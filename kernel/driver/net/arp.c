#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include "arp.h"

arp_cache_t arp_cache[1024];

void init_arp()
{
    unsigned char everyone[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    //unsigned char ip[4]={208,67,222,222};
    memset(arp_cache, 0, sizeof(arp_cache_t)*1024);
    // ARP breadcast
    printf("[ARP] Breadcast\n");
    arp_request(router_ip, everyone);
    sleep(1000);
}

int arp_save_address(unsigned char *ip, unsigned char *mac)
{

    for(int i=0; i < 1024; i++){
    

        if(arp_cache[i].ip[0] == 0)
            continue;
  
        if( (ip[0] == arp_cache[i].ip[0]) && (ip[1] == arp_cache[i].ip[1])\
        && (ip[2] == arp_cache[i].ip[2]) && (ip[3] == arp_cache[i].ip[3]) ){

            if( (mac[0] == arp_cache[i].mac[0]) && (mac[1] == arp_cache[i].mac[1])\
            && (mac[2] == arp_cache[i].mac[2]) && (mac[3] == arp_cache[i].mac[3])\
            && (mac[4] == arp_cache[i].mac[4]) && (mac[5] == arp_cache[i].mac[5])){

                return 0;
            }

        }
    }
    
    for(int i=0; i < 1024; i++){
        if( (arp_cache[i].ip[0] + arp_cache[i].ip[1] + arp_cache[i].ip[2] + arp_cache[i].ip[3]) == 0){

            fillMac(arp_cache[i].mac, mac);
            fillIP(arp_cache[i].ip, ip);
            break;
        }
    }

    return 1;
}

static unsigned char *arp_cache_get_address(unsigned char *mac, unsigned char *ip){

    for(int i=0; i < 1024; i++){
        if( (ip[0] == arp_cache[i].ip[0]) && (ip[1] == arp_cache[i].ip[1])\
        && (ip[2] == arp_cache[i].ip[2]) && (ip[3] == arp_cache[i].ip[3]) ){

            fillMac(mac, arp_cache[i].mac);    
            return mac;
        }
    }

    return 0;
}

int get_hardwere_ethernet(unsigned char *mac, unsigned int ip){
    unsigned char everyone[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    int n = 2;
    while(n > 0) {
        if( !arp_cache_get_address(mac, router_ip) ) {
            // broadcast
            arp_request(router_ip, everyone);
            sleep(1000);
        }else return 0;
    }

    return -1;
}


void arp_request(unsigned char *ip, unsigned char *mac)
{
    unsigned long addr = (unsigned long) malloc(sizeof(arp_header_t) + sizeof(ether_header_t));
    ether_header_t *eh = (ether_header_t *)addr;
    arp_header_t *arp = (arp_header_t*) (addr + sizeof(ether_header_t));
    unsigned char empty[SIZE_OF_MAC] = {0x00,0x00,0x00,0x00,0x00,0x00};

    // Ethernet header
    fillMac(eh->dst, mac);
    fillMac(eh->src, default_ethernet_device.mac);
    eh->type = htons(ET_ARP);

    // arp header
    arp->hardware_type = htons(0x0001);
    arp->protocol_type = htons(ET_IPV4);
    //Hardware address length (MAC)
    arp->hardware_size = 6;
    //Protocol address length (IP)    
    arp->protocol_size = 4;
    // Replay (2)
    arp->operation = htons(ARP_OPC_REQUEST);

    // Sender MAC Address
    fillMac(arp->source_mac, default_ethernet_device.mac);
    // Sender IP Address
    fillIP(arp->source_ip, our_ip);
    // Target MAC Address
    fillMac(arp->dest_mac, empty);
    // Target IP Address
    fillIP(arp->dest_ip, ip);

    // send package
    if( send_ethernet_package((void*)addr, sizeof(arp_header_t) + sizeof(ether_header_t)) ){
        printf("[ETH] ARP request error\n");
    }

    free(arp);
}


void arp_replay(unsigned char *ip, unsigned char *mac)
{
    unsigned long addr = (unsigned long) malloc(sizeof(arp_header_t) + sizeof(ether_header_t));
    ether_header_t *eh = (ether_header_t *)addr;
    arp_header_t *arp = (arp_header_t*) (addr + sizeof(ether_header_t));

    // Ethernet header
    fillMac(eh->dst, mac);
    fillMac(eh->src, default_ethernet_device.mac);
    eh->type = htons(ET_ARP);

    // arp header
    arp->hardware_type = htons(0x0001);
    arp->protocol_type = htons(ET_IPV4);
    //Hardware address length (MAC)
    arp->hardware_size = 6;
    //Protocol address length (IP)    
    arp->protocol_size = 4;
    // Replay (2)
    arp->operation = htons(ARP_OPC_REPLY);

    // Sender MAC Address
    fillMac(arp->source_mac, default_ethernet_device.mac);
    // Sender IP Address
    fillIP(arp->source_ip, our_ip);
    // Target MAC Address
    fillMac(arp->dest_mac, mac);
    // Target IP Address
    fillIP(arp->dest_ip, ip);

    // send package
    if( send_ethernet_package((void*)addr, sizeof( arp_header_t ) + sizeof(ether_header_t)) ){
        printf("[ETH] ARP replay error\n");
    }

    free(arp);
}

void arp_replay2(unsigned char *src_ip, unsigned char *dest_ip, unsigned char *dest_mac)
{
    unsigned long addr = (unsigned long) malloc(sizeof(arp_header_t) + sizeof(ether_header_t));
    ether_header_t *eh = (ether_header_t *)addr;
    arp_header_t *arp = (arp_header_t*) (addr + sizeof(ether_header_t));
 
    // Ethernet header
    fillMac(eh->dst, dest_mac);
    fillMac(eh->src, default_ethernet_device.mac);
    eh->type = htons(ET_ARP);

    // arp header
    arp->hardware_type = htons(0x0001);
    arp->protocol_type = htons(ET_IPV4);
    //Hardware address length (MAC)
    arp->hardware_size = 6;
    //Protocol address length (IP)    
    arp->protocol_size = 4;
    // Replay (2)
    arp->operation = htons(ARP_OPC_REPLY);

    // Sender MAC Address
    fillMac(arp->source_mac, default_ethernet_device.mac);
    // Sender IP Address
    fillIP(arp->source_ip, src_ip);
    // Target MAC Address
    fillMac(arp->dest_mac, dest_mac);
    // Target IP Address
    fillIP(arp->dest_ip, dest_ip);

    // send package
    if( send_ethernet_package((void*)addr, sizeof( arp_header_t ) + sizeof(ether_header_t) ) ){
        printf("[ETH] ARP replay error\n");
    }

    free(arp);
}

void arp_hexdump(arp_header_t *arp, ether_header_t *eh){
    printf("Ethernet\n");
    printf("\tDestination: %x:%x:%x:%x:%x:%x\n",eh->dst[0],eh->dst[1],eh->dst[2],eh->dst[3],eh->dst[4],eh->dst[5]);
    printf("\tSource: %x:%x:%x:%x:%x:%x\n",eh->src[0],eh->src[1],eh->src[2],eh->src[3],eh->src[4],eh->src[5]);    
    printf("\tType: %x\n", htons(eh->type));

    printf("Address Resolution Protocol\n");
    printf("\tHardware type: %x\n", htons(arp->hardware_type));
    printf("\tProtocol type: %x\n", htons(arp->protocol_type));
    printf("\tHardware size: %x\n", arp->hardware_size);
    printf("\tProtocol size: %x\n", arp->protocol_size);
    printf("\tOpcode: %x\n", htons(arp->operation));

    printf("\tSender MAC address: %x:%x:%x:%x:%x:%x\n",arp->source_mac[0],arp->source_mac[1],
    arp->source_mac[2],arp->source_mac[3],arp->source_mac[4],arp->source_mac[5]);
    printf("\tSender IP address: %d.%d.%d.%d\n",arp->source_ip[0],arp->source_ip[1],arp->source_ip[2],arp->source_ip[3]);

    printf("\tTarget MAC address: %x:%x:%x:%x:%x:%x\n",arp->dest_mac[0],arp->dest_mac[1],
    arp->dest_mac[2],arp->dest_mac[3],arp->dest_mac[4],arp->dest_mac[5]);
    printf("\tTarget IP address: %d.%d.%d.%d\n",arp->dest_ip[0],arp->dest_ip[1],arp->dest_ip[2],arp->dest_ip[3]);

}
