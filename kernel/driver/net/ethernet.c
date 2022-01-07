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


int ethernet_filtr(unsigned char *mac);

ethernet_device_t default_ethernet_device;
unsigned long package_recieved_ack = 0;
ethernet_package_descriptor_t default_ethernet_package_descriptor;

unsigned short switch_endian16(unsigned short nb) {
    return (nb>>8) | (nb<<8);
}

unsigned int switch_endian32(unsigned int nb) {
       return ((nb>>24)&0xff)      |
              ((nb<<8)&0xff0000)   |
              ((nb>>8)&0xff00)     |
              ((nb<<24)&0xff000000);
   }

ethernet_device_t get_default_ethernet_device(){
    return default_ethernet_device;
}

void ethernet_set_link_status(unsigned long a){
    default_ethernet_device.is_online = a;
}


void register_ethernet_device(unsigned long send_package,unsigned long receive_package,
unsigned long receive_package_handler, unsigned char mac[SIZE_OF_MAC])
{
    default_ethernet_device.receive_package = receive_package;
    default_ethernet_device.receive_package_handler = receive_package_handler;
    default_ethernet_device.send_package = send_package;
    
    default_ethernet_device.is_enabled = 1;

    for(int i = 0 ; i < SIZE_OF_MAC ; i++){
        default_ethernet_device.mac[i] = mac[i];
    }
}



ethernet_package_descriptor_t get_ethernet_package_handler(){
    ethernet_package_descriptor_t (*get_package)() = (void*)default_ethernet_device.receive_package_handler;
    return get_package();
}

ethernet_package_descriptor_t get_ethernet_package(){
    ethernet_package_descriptor_t (*get_package)() = (void*)default_ethernet_device.receive_package;
    return get_package();
}

int send_ethernet_package(ethernet_package_descriptor_t desc){
    int (*send_package)(ethernet_package_descriptor_t desc) = (void*)default_ethernet_device.send_package;
    return send_package(desc);
}


void handler_ethernet_package_received(){

    if(!default_ethernet_device.is_online) return;

    ether_header_t *eh;
    arp_header_t *arp;
    ipv4_header_t *ipv4;
    udp_header_t *udp;

    char buf[256];
    char *data;
    int len;
    char string[] = "Nelson";

    unsigned char mac[SIZE_OF_MAC] = {0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char ip[SIZE_OF_IP] = {0,0,0,0};

    package_recieved_ack = 0;

    ethernet_package_descriptor_t prd;
    prd = get_ethernet_package_handler();

    if(prd.buf == 0 || prd.buffersize == 0) return;

    eh = (ether_header_t*) prd.buf;
    
    switch( htons(eh->type) ){
        case ET_ARP:
            arp = (arp_header_t*) prd.buf;
            fillMac(mac, arp->source_mac);
            fillIP(ip, arp->source_ip);

            switch( htons(arp->operation) ){
                case ARP_OPC_REQUEST:
                    printf("ARP REQUEST\n");
                    arp_save_address( ip, mac);
                    arp_replay(ip, mac);
                    break;
                case ARP_OPC_REPLY:
                    printf("ARP REPLAY\n");
                    arp_save_address( ip, mac);
                    break;
            }   
            break;
        case ET_IPV4:

            /*if( ethernet_filtr( eh->dst) ){
                return;
            } */

            ipv4 = (ipv4_header_t*) prd.buf;
            switch( ipv4->protocol){
                case IP_PROTOCOL_ICMP:
                    printf("IPV4 ICMP\n");
                    break;
                case IP_PROTOCOL_TCP:
                    printf("IPV4 TCP\n");
                    break;
                case IP_PROTOCOL_UDP:
                    printf("IPV4 UDP\n");
                    udp = (udp_header_t*) ((unsigned long)prd.buf + sizeof(ipv4_header_t));
                    data = (char*) udp;
                    data += sizeof(udp_header_t);

                    memset(buf, 0, 256);
                    len = htons(udp->length) - sizeof(udp_header_t);
                    memcpy(buf, data , len);

                    fillIP(ip, (unsigned char*)&ipv4->src);
                    printf("Client IP address: %d.%d.%d.%d UDP Source Port address: %d\n",ip[0],ip[1],ip[2],ip[3],
                    htons(udp->src_port));

                    
                    //udp_send( ipv4->src, htons(udp->src_port), string, 6);

                    printf("Data: %s\n", buf);

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
    
}

int int_ethernet()
{
    int bus, slot, function;
    unsigned short device, vendor;
    unsigned int data = pci_scan_class(2);

    memset((char*)&default_ethernet_device, 0, sizeof(ethernet_device_t));

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
    
    
    if( (device == 0x100e) || (device == 0x153A) || (device == 0x10EA) /*|| (vendor == 0x8086)*/) {
        init_e1000( bus, slot, function );
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

void fillEthernetHeader(struct EthernetHeader* eh, unsigned char* dest_mac,unsigned short type){
    fillMac((unsigned char*)&eh->to,dest_mac);
    fillMac((unsigned char*)&eh->from,(unsigned char*)&default_ethernet_device.mac);
    eh->type = type;
}

void fillIpv4Header(struct IPv4Header *ipv4header, unsigned char* destmac, 
unsigned short length,unsigned char protocol,unsigned int from, unsigned int to){
    fillEthernetHeader((struct EthernetHeader*)&ipv4header->ethernetheader,destmac,ETHERNET_TYPE_IP4);
    ipv4header->version = 4;
    ipv4header->internet_header_length = 5;
    ipv4header->type_of_service = 0;
    ipv4header->total_length = switch_endian16( length );
    ipv4header->id = switch_endian16(1);
    ipv4header->flags = 0;
    ipv4header->fragment_offset= 0b01000;
    ipv4header->time_to_live = 64;
    ipv4header->protocol = protocol;
    ipv4header->checksum = 0;
    ipv4header->source_addr = from;
    ipv4header->dest_addr = to;

    unsigned int checksum = 0;
    checksum += 0x4500;
    checksum += length;
    checksum += 1;
    checksum += 0x4000;
    checksum += 0x4000 + protocol;
    checksum += (from >> 16) & 0xFFFF;
    checksum += from & 0xFFFF; 
    checksum += (to >> 16) & 0xFFFF;
    checksum += to & 0xFFFF;
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    ipv4header->checksum = switch_endian16((unsigned short) (~checksum));
}

void fillUdpHeader(struct UDPHeader *udpheader, unsigned char *destmac, unsigned short size,
unsigned int from, unsigned int to,unsigned short source_port, unsigned short destination_port){
    fillIpv4Header((struct IPv4Header*)&udpheader->ipv4header,destmac,size,17,from,to);

    udpheader->length = switch_endian16(size - (sizeof(struct IPv4Header)-sizeof(struct EthernetHeader)));
    udpheader->destination_port = switch_endian16(destination_port);
    udpheader->source_port = switch_endian16(source_port);

    unsigned short checksum = 0;
    udpheader->checksum = checksum;
}

void fillDhcpDiscoverHeader(struct DHCPDISCOVERHeader *dhcpheader){
    unsigned char destmac[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    unsigned short size = sizeof(struct DHCPDISCOVERHeader) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&dhcpheader->udpheader,(unsigned char*)&destmac,size,0,0xFFFFFFFF,68,67);
}

void fillDhcpRequestHeader(struct DHCPREQUESTHeader *dhcpheader){
    unsigned char destmac[SIZE_OF_MAC] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    unsigned short size = sizeof(struct DHCPREQUESTHeader) - sizeof(struct EthernetHeader);
    fillUdpHeader((struct UDPHeader*)&dhcpheader->udpheader,(unsigned char*)&destmac,size,0,0xFFFFFFFF,68,67);
}

/*
`* |-------------------------------------|
 * |Ethernet header (14 bytes)           |
 * |-------------------------------------|
 * |IPv4 header (20 bytes)               |
 * |-------------------------------------|
 * |UDP header (8 bytes)                 |
 * |-------------------------------------|
 * |The DHCP request (300 bytes payload) |
 * |-------------------------------------|
 */
unsigned char* getIpAddressFromDHCPServer(){
    struct DHCPDISCOVERHeader *dhcpheader = (struct DHCPDISCOVERHeader *)malloc(sizeof(struct DHCPDISCOVERHeader));
    dhcpheader->op = 1;
    dhcpheader->htype = 1;
    dhcpheader->hlen = 6;
    dhcpheader->hops = 0;
    dhcpheader->xid = 0x26F30339;
    dhcpheader->timing = 0;
    dhcpheader->flags = switch_endian16(0x8000);

    fillMac((unsigned char*)&dhcpheader->client_mac_addr,(unsigned char*)&default_ethernet_device.mac);
    dhcpheader->magic_cookie = 0x63538263;
    // DHCP Message Type
    dhcpheader->options[0] = 0x35;
    dhcpheader->options[1] = 0x01;
    dhcpheader->options[2] = 0x01;
    // parameter request list
    dhcpheader->options[3] = 0x37;
    dhcpheader->options[4] = 0x40;
    dhcpheader->options[5] = 0xfc;
    for(unsigned char i = 1 ; i < 0x43 ; i++){
        dhcpheader->options[5+i] = i;
    }
    // dhcpheader->options[68] = 0x00;
    // ip address lease time
    dhcpheader->options[69] = 0x33;
    dhcpheader->options[70] = 0x04;
    dhcpheader->options[71] = 0x00;
    dhcpheader->options[72] = 0x00;
    dhcpheader->options[73] = 0x00;
    dhcpheader->options[74] = 0x01;
    // end
    dhcpheader->options[75] = 0xFF;
    
    fillDhcpDiscoverHeader(dhcpheader);
    ethernet_package_descriptor_t sec;
    sec.buffersize = sizeof(struct DHCPDISCOVERHeader);
    // TODO
    // Modificado por mim
    unsigned long long addr = (unsigned long long)dhcpheader;
    sec.buf = (void*) addr;

    printf("[ETH] Send DHCP Discover\n");
    int res_fs = send_ethernet_package(sec); // send package
    if(res_fs){
        free(dhcpheader);

        printf("Error\n");
        return 0;
    }

    ethernet_package_descriptor_t prd;
    while(1){
        prd = get_ethernet_package();
        if(prd.flag) {
            continue;
        } 
        struct EthernetHeader *eh = (struct EthernetHeader*) prd.buf;
        if(eh->type==ETHERNET_TYPE_IP4){
            struct DHCPDISCOVERHeader *hd5 = ( struct DHCPDISCOVERHeader*) prd.buf;
            if(hd5->options[2]==2&&hd5->xid==dhcpheader->xid){
                break;
            }
        } 
    }
    printf("[ETH] DHCP Offer\n");
    struct DHCPDISCOVERHeader *hd = ( struct DHCPDISCOVERHeader*) prd.buf;
    unsigned char* offeredip = (unsigned char*) &hd->dhcp_offered_machine;

    // Configure IP device
    // Nelson
    fillIP((unsigned char*)&default_ethernet_device.client_ip,(unsigned char*)&hd->dhcp_offered_machine);
    fillIP((unsigned char*)&default_ethernet_device.server_ip,(unsigned char*)&hd->ip_addr_of_dhcp_server);

    free(dhcpheader);

    struct DHCPREQUESTHeader *dhcp2header = (struct DHCPREQUESTHeader *)malloc(sizeof(struct DHCPREQUESTHeader));
    dhcp2header->op = 1;
    dhcp2header->htype = 1;
    dhcp2header->hlen = 6;
    dhcp2header->hops = 0;
    dhcp2header->xid = 0x2CF30339;
    dhcp2header->timing = 0;
    dhcp2header->flags = switch_endian16(0x8000);

    fillMac((unsigned char*)&dhcp2header->client_mac_addr,(unsigned char*)&default_ethernet_device.mac);
    dhcp2header->magic_cookie = 0x63538263;

    // DHCP Message Type
    dhcp2header->options[0] = 0x35;
    dhcp2header->options[1] = 0x01;
    dhcp2header->options[2] = 0x03;
    // Client identifier
    dhcp2header->options[3] = 0x3d;
    dhcp2header->options[4] = 0x07;
    dhcp2header->options[5] = 0x01;
    fillMac((unsigned char*)(&dhcp2header->options)+6,(unsigned char*)&default_ethernet_device.mac);
    // Requested IP addr
    dhcp2header->options[12] = 0x32;
    dhcp2header->options[13] = 0x04;
    fillMac((unsigned char*)(&dhcp2header->options)+14,offeredip);
    // DHCP Server identifier
    dhcp2header->options[18] = 0x36;
    dhcp2header->options[19] = 0x04;
    fillMac((unsigned char*)(&dhcp2header->options)+20,(unsigned char*)&hd->ip_addr_of_dhcp_server);
    dhcp2header->options[24] = 0xFF;

    fillDhcpRequestHeader(dhcp2header);

    ethernet_package_descriptor_t s3c;
    s3c.buffersize = sizeof(struct DHCPREQUESTHeader);
    addr = (unsigned long long)dhcp2header;
    s3c.buf = (void*)addr;

    printf("[ETH] Send DHCP Request\n");
    if( send_ethernet_package(s3c) ) { // send package
        free(dhcp2header);
        printf("Error\n");
        return offeredip;
    }

    ethernet_package_descriptor_t p3d;
    while(1){
        p3d = get_ethernet_package();
        if(p3d.flag) {
            continue;
        }
 
        struct EthernetHeader *eh = (struct EthernetHeader*) p3d.buf;
        if(eh->type==ETHERNET_TYPE_IP4){
            struct DHCPDISCOVERHeader *hd5 = ( struct DHCPDISCOVERHeader*) p3d.buf;
            if(hd5->options[2]==5&&hd5->xid==dhcp2header->xid){
                break;
            }
        } 
    }

    free(dhcp2header);

    return offeredip;
}



void initialise_ethernet(){

    unsigned char our_ip[SIZE_OF_IP];

    package_recieved_ack = 1;

    printf("[ETH] Ethernet module reached!\n");
    ethernet_device_t ed = get_default_ethernet_device();
    if(ed.is_enabled){
        printf("[ETH] There is a ethernet device present on the system!\n");
        printf("[ETH] Asking DHCP server for our address....\n");
        unsigned char *dhcpid = getIpAddressFromDHCPServer();
        if(dhcpid){
            fillIP((unsigned char*)&our_ip,dhcpid);
            printf("[ETH] DHCP is present\n");
        }else{
            printf("[ETH] No DHCP server present here, using static address\n");
            unsigned char dinges[8] = {0,0,0,0};   
            fillIP((unsigned char*)&our_ip,(unsigned char*)&dinges);
        }


        fillIP((unsigned char*)&our_ip,(unsigned char*)&default_ethernet_device.client_ip);
        printf("[ETH] Client IP %d.%d.%d.%d \n",our_ip[0], our_ip[1], our_ip[2], our_ip[3]);
        fillIP((unsigned char*)&our_ip,(unsigned char*)&default_ethernet_device.server_ip);
        printf("[ETH] Server IP %d.%d.%d.%d \n",our_ip[0], our_ip[1], our_ip[2], our_ip[3]);

        ethernet_set_link_status(1);

        // ARP CACHE
        init_arp();
        sleep(2000);

        unsigned char ip[SIZE_OF_IP] = {192,168,43,1}; //{100,68,106,99};
        unsigned int dest_ip = 0;
        fillIP((unsigned char*)&dest_ip, ip);
     
        char string[] = "Nelson";
        udp_send( dest_ip, 20001, string, 6);

    }
}

int ethernet_filtr(unsigned char *mac)
{
    if((default_ethernet_device.mac[0] == mac[0]) && (default_ethernet_device.mac[1] == mac[1])\
    && (default_ethernet_device.mac[2] == mac[2]) && (default_ethernet_device.mac[3] == mac[3])\
    && (default_ethernet_device.mac[4] == mac[4]) && (default_ethernet_device.mac[5] == mac[5])){

        return 0;
    }

    
    return -1;
}
