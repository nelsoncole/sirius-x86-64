#ifndef __ETHERNET_H__
#define __ETHERNET_H__

/////////////
// ETHERNET
////////////


// DRIVER
typedef struct _ethernet_device{
    unsigned char mac[6];
    unsigned char ip[4];
    unsigned char is_enabled;
    unsigned char is_online;
    //
    unsigned long send_package;
    unsigned long receive_package;

}__attribute__((packed)) ethernet_device_t;

typedef struct _ethernet_package_descriptor{
	unsigned int buffersize;
	void *buf;
}__attribute__((packed)) ethernet_package_descriptor_t;

// ETHERNET

#define SIZE_OF_MAC 6
#define SIZE_OF_IP 4
#define ETHERNET_TYPE_ARP 0x0608
#define ETHERNET_TYPE_IP4 0x0008

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


void init_e1000(int bus,int slot,int function);
int int_ethernet();
void register_ethernet_device(unsigned long send_package,unsigned long receive_package,unsigned char mac[SIZE_OF_MAC]);
void initialise_ethernet();



#endif
