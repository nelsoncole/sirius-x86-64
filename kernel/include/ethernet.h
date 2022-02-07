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


// Flags
#define TCP_FIN     (1 << 0)
#define TCP_SYN     (1 << 1)
#define TCP_RST     (1 << 2)
#define TCP_PSH     (1 << 3)
#define TCP_ACK     (1 << 4)
#define TCP_URG     (1 << 5)


extern unsigned char our_ip[SIZE_OF_IP];
extern unsigned char router_ip[SIZE_OF_IP];
extern unsigned char dns_ip[SIZE_OF_IP];
extern unsigned char dhcp_ip[SIZE_OF_IP];


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
    unsigned char is_interrupt;
    //
    unsigned long send_package;
    unsigned long receive_package;

}__attribute__((packed)) ethernet_device_t;

typedef struct _ethernet_package_descriptor{
    unsigned int flag;
    unsigned int count;
	unsigned int buffersize;
	void *buf;

}__attribute__((packed)) ethernet_package_descriptor_t;

// ETHERNET


///
extern ethernet_package_descriptor_t packege_desc_buffer[32];


///
void fillMac(unsigned char* to,unsigned char* from);
void fillIP(unsigned char* to,unsigned char* from);

extern ethernet_device_t default_ethernet_device;

ethernet_package_descriptor_t *get_ethernet_package();
int send_ethernet_package(const void *buf, size_t size);
void handler_ethernet_package_received();

void init_e1000(int bus,int slot,int function);
void setup_realtek( int bus, int slot, int function);
void setup_pcnet( int bus, int slot, int function );
int int_ethernet_device();
void register_ethernet_device(unsigned long send_package,unsigned long receive_package,unsigned char mac[SIZE_OF_MAC]);
void initialise_ethernet();

//
void init_arp();
int arp_save_address(unsigned char *ip, unsigned char *mac);
int get_hardwere_ethernet(unsigned char *mac, unsigned int ip);
void arp_request(unsigned char *ip, unsigned char *mac);
void arp_replay(unsigned char *ip, unsigned char *mac);
void arp_replay2(unsigned char *src_ip, unsigned char *dest_ip, unsigned char *dest_mac);
//
int ipv4_send(void *buf, unsigned char protocol, unsigned int src_address, unsigned int dst_address, unsigned length);
//
int udp_send(unsigned int src_address, unsigned int dst_address, unsigned short src_port, unsigned short dst_port, const void *data, size_t length);
int udp_receive(void *data, size_t length, unsigned short port);
//
int tcp_send(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq, unsigned int ack, unsigned char flags, const void *data, size_t length);
int init_tcp();
int tcp_connect(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port);
int tcp_push_ack(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int seq,
unsigned int ack, unsigned char flags, size_t len);
int tcp_send_payload(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned char flags, const void *data, size_t len);
void tcp_finish(unsigned int src_address, unsigned int dst_address,
unsigned short src_port, unsigned short dst_port, unsigned int ack);

//
int dhcp_send(unsigned char *your_ip, unsigned char *server_ip, unsigned char message_type);
int dhcp_parse_options(const void *buf);



#endif
