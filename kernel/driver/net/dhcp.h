#ifndef __DHCP_H__
#define __DHCP_H__

// Message Types

#define DHCP_DISCOVER                   1
#define DHCP_OFFER                      2
#define DHCP_REQUEST                    3
#define DHCP_DECLINE                    4
#define DHCP_ACK                        5
#define DHCP_NAK                        6
#define DHCP_RELEASE                    7
#define DHCP_INFORM                     8

#define OPT_PAD                         0
#define OPT_SUBNET_MASK                 1
#define OPT_ROUTER                      3
#define OPT_DNS                         6
#define OPT_REQUESTED_IP_ADDR           50
#define OPT_LEASE_TIME                  51
#define OPT_DHCP_MESSAGE_TYPE           53
#define OPT_SERVER_ID                   54
#define OPT_PARAMETER_REQUEST           55
#define OPT_END                         255

typedef struct __dhcp_header{
    unsigned char   op;
    unsigned char   htype;
    unsigned char   hlen;
    unsigned char   hops;
    unsigned int    xid;
    unsigned short  secs;   //timing
    unsigned short  flags; 
    unsigned int    ciaddr; // Client IP Address
    unsigned int    yiaddr; // Your IP Address
    unsigned int    siaddr; // Server IP Address
    unsigned int    giaddr; // Gateway IP Address switched by relay
    unsigned char   chaddr[16]; // Client Hardware Address
    unsigned char   sname [64];
    unsigned char   file [128];
    unsigned int    magic_cookie;
    unsigned char   options[312 - 4];
} __attribute__ ((packed)) dhcp_header_t;

extern const char *string_dhcp_message[9];


#endif
