#ifndef __ETHERNET_H__
#define __ETHERNET_H__

/////////////
// ETHERNET
////////////

#include <stddef.h>

#define ET_IPV4                         0x0800
#define ET_ARP                          0x0806
#define ET_IPV6                         0x86DD


// ARP

#define ARP_OPC_REQUEST  0x01
#define ARP_OPC_REPLY    0x02

// IP Protocols

#define IP_PROTOCOL_ICMP                1
#define IP_PROTOCOL_TCP                 6
#define IP_PROTOCOL_UDP                 17



#define SIZE_OF_MAC 6
#define SIZE_OF_IP 4


#define ETHERNET_TYPE_ARP 0x0608
#define ETHERNET_TYPE_IP4 0x0008



static inline unsigned short htons(unsigned short nb)
{
    return (nb>>8) | (nb<<8);
}

static inline unsigned int htonl(unsigned int nb) {
       return ((nb>>24)&0xff)      |
              ((nb<<8)&0xff0000)   |
              ((nb>>8)&0xff00)     |
              ((nb<<24)&0xff000000);
}

// DRIVER
typedef struct _ethernet_device{
    unsigned char mac[6];
    unsigned char client_ip[4];
    unsigned char server_ip[4];
    unsigned char is_enabled;
    unsigned char is_online;
    //
    unsigned long send_package;
    unsigned long receive_package;

}__attribute__((packed)) ethernet_device_t;

typedef struct _ethernet_package_descriptor{
    unsigned int flag;
	unsigned int buffersize;
	void *buf;
    
}__attribute__((packed)) ethernet_package_descriptor_t;

// ETHERNET

struct EthernetHeader{
    unsigned char to[SIZE_OF_MAC];
    unsigned char from[SIZE_OF_MAC];
    unsigned short type;
} __attribute__ ((packed));

struct ARPHeader{
    struct EthernetHeader ethernetheader;
    unsigned short hardware_type;
    unsigned short protocol_type;
    unsigned char hardware_address_length;
    unsigned char protocol_address_length;
    unsigned short operation;

    unsigned char source_mac[SIZE_OF_MAC];
    unsigned char source_ip[SIZE_OF_IP];

    unsigned char dest_mac[SIZE_OF_MAC];
    unsigned char dest_ip[SIZE_OF_IP];
} __attribute__ ((packed));

struct IPv4Header{
    struct EthernetHeader ethernetheader;
    unsigned char internet_header_length:4;
    unsigned char version:4;
    unsigned char type_of_service;
    unsigned short total_length;
    unsigned short id;
    unsigned short flags:3;
    unsigned short fragment_offset:13;
    unsigned char time_to_live;
    unsigned char protocol;
    unsigned short checksum;
    unsigned int source_addr;
    unsigned int dest_addr;
} __attribute__ ((packed));

struct UDPHeader{
    struct IPv4Header ipv4header;
    unsigned short source_port;
    unsigned short destination_port;
    unsigned short length;
    unsigned short checksum;
} __attribute__ ((packed));

struct DHCPDISCOVERHeader{
    struct UDPHeader udpheader;
    unsigned char op;
    unsigned char htype;
    unsigned char hlen;
    unsigned char hops;
    unsigned int xid;
    unsigned short timing;
    unsigned short flags;
    unsigned int address_of_machine;
    unsigned int dhcp_offered_machine;
    unsigned int ip_addr_of_dhcp_server;
    unsigned int ip_addr_of_relay;
    unsigned char client_mac_addr [16];
    unsigned char sname [64];
    unsigned char file [128];
    unsigned int magic_cookie;
    unsigned char options[76];
} __attribute__ ((packed));

struct DHCPREQUESTHeader{
    struct UDPHeader udpheader;
    unsigned char op;
    unsigned char htype;
    unsigned char hlen;
    unsigned char hops;
    unsigned int xid;
    unsigned short timing;
    unsigned short flags;
    unsigned int address_of_machine;
    unsigned int dhcp_offered_machine;
    unsigned int ip_addr_of_dhcp_server;
    unsigned int ip_addr_of_relay;
    unsigned char client_mac_addr [16];
    unsigned char sname [64];
    unsigned char file [128];
    unsigned int magic_cookie;
    unsigned char options[25];
} __attribute__ ((packed));

///
unsigned short switch_endian16(unsigned short nb);
unsigned int switch_endian32(unsigned int nb);


///
void fillMac(unsigned char* to,unsigned char* from);
void fillIP(unsigned char* to,unsigned char* from);
void fillEthernetHeader(struct EthernetHeader* eh, unsigned char* dest_mac,unsigned short type);
void fillIpv4Header(struct IPv4Header *ipv4header, unsigned char* destmac, 
unsigned short length,unsigned char protocol,unsigned int from, unsigned int to);
void fillUdpHeader(struct UDPHeader *udpheader, unsigned char *destmac, unsigned short size,
unsigned int from, unsigned int to,unsigned short source_port, unsigned short destination_port);
void fillDhcpDiscoverHeader(struct DHCPDISCOVERHeader *dhcpheader);
void fillDhcpRequestHeader(struct DHCPREQUESTHeader *dhcpheader);


extern ethernet_device_t default_ethernet_device;

ethernet_package_descriptor_t get_ethernet_package();
int send_ethernet_package(const void *buf, size_t size);
void handler_ethernet_package_received();

void init_e1000(int bus,int slot,int function);
int int_ethernet();
void register_ethernet_device(unsigned long send_package,unsigned long receive_package,unsigned char mac[SIZE_OF_MAC]);
void initialise_ethernet();

//
void init_arp();
int arp_save_address(unsigned char *ip, unsigned char *mac);
unsigned char *arp_get_address(unsigned char *mac, unsigned char *ip);
void arp_request(unsigned char *ip, unsigned char *mac);
void arp_replay(unsigned char *ip, unsigned char *mac);


int ipv4_send(void *buf, unsigned char protocol, unsigned char *mac, unsigned int src_address, unsigned int dst_address, unsigned length);
unsigned short net_checksum(const unsigned char *data, const unsigned char *end);

int udp_send(unsigned int src_address, unsigned int dst_address, unsigned short src_port, unsigned short dst_port, const void *data, size_t length);


//
int dhcp_send(unsigned char message_type);



#endif
