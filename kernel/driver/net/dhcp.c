#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include "udp.h"
#include "dhcp.h"


const char *string_dhcp_message[9]={
0,"DHCP DISCOVER", "DHCP OFFER","DHCP REQUEST", "DHCP DECLINE",
"DHCP ACK", "DHCP NAK", "DHCP RELEASE", "DHCP INFORM"};


int dhcp_send(unsigned char *your_ip, unsigned char *server_ip, unsigned char message_type)
{
    int opt_size = 0;
    dhcp_header_t *dhcp = (dhcp_header_t*)malloc(sizeof(dhcp_header_t));

    dhcp->op    = 1;
    dhcp->htype = 1;
    dhcp->hlen  = 6;
    dhcp->hops  = 0;
    dhcp->xid   = htonl(0x3903F326);
    dhcp->secs  = htons(0);
    dhcp->flags = htons(0x0000);
    dhcp->ciaddr= 0;
    dhcp->yiaddr= 0;
    dhcp->siaddr = 0;
    dhcp->giaddr = 0;

    fillMac(dhcp->chaddr, default_ethernet_device.mac);
    memset(dhcp->sname, 0, 64);
    memset(dhcp->file, 0, 128);
    dhcp->magic_cookie = htonl(0x63825363);

    dhcp->options[0] = OPT_DHCP_MESSAGE_TYPE;
    dhcp->options[1] = 0x01;
    // DHCP Message Type
    dhcp->options[2] = message_type;

    switch(message_type){
        case DHCP_DISCOVER:
            printf("DHCP DISCOVERY\n");
            // Parameter Request list
            dhcp->options[3] = OPT_PARAMETER_REQUEST;
            dhcp->options[4] = 3;
            dhcp->options[5] = OPT_SUBNET_MASK;
            dhcp->options[6] = OPT_ROUTER;
            dhcp->options[7] = OPT_DNS;
            // Option End
            dhcp->options[8] = OPT_END;

            opt_size = 9;
            break;
        case DHCP_REQUEST:
            printf("DHCP REQUEST\n");
            // Requested IP address
            dhcp->options[3] = OPT_REQUESTED_IP_ADDR;
            dhcp->options[4] = SIZE_OF_IP;
            dhcp->options[5] = your_ip[0];
            dhcp->options[6] = your_ip[1];
            dhcp->options[7] = your_ip[2];
            dhcp->options[8] = your_ip[3];
            // Server Identifier
            dhcp->options[9] = OPT_SERVER_ID;
            dhcp->options[10]= SIZE_OF_IP;
            dhcp->options[11]= server_ip[0];
            dhcp->options[12]= server_ip[1];
            dhcp->options[13]= server_ip[2];
            dhcp->options[14]= server_ip[3];
            // Parameter Request list 
            dhcp->options[15]= OPT_PARAMETER_REQUEST;
            dhcp->options[16]= 3;
            dhcp->options[17]= OPT_SUBNET_MASK;
            dhcp->options[18]= OPT_ROUTER;
            dhcp->options[19]= OPT_DNS;
            // Option End
            dhcp->options[20]= OPT_END;
            opt_size = 21;
            break;
        default:
            printf("DHCP UNKNOWN\n");
            break;   
    }

    
    int len = udp_send( 0x00000000, 0xFFFFFFFF, 68, 67, dhcp, sizeof(dhcp_header_t) - 308 + opt_size);
    if( !len ){
        printf("DHCP error\n");
    }

    free(dhcp);
    return len;
}


int dhcp_parse_options(const void *buf){
    dhcp_header_t *dhcp = (dhcp_header_t *)buf;
    
    if(htonl(dhcp->magic_cookie) != 0x63825363) return -1;

    int ip_size;
    unsigned char opt_type;
    fillIP((unsigned char*)&our_ip,(unsigned char*)&dhcp->yiaddr);
    int i =0;
    while(1){
        opt_type = dhcp->options[i++];
        if(/*opt_type == OPT_END ||*/ i >= 308 ) break;

        // GATEWAY
        if(opt_type == OPT_ROUTER){
            ip_size = dhcp->options[i++];
            switch(ip_size){
                case 4:
                    fillIP((unsigned char*)&router_ip, (unsigned char*)&dhcp->options[i]);
                    break;  
                default:
                    printf("No IPv4 is %d\n", ip_size);
                    break;
            }
            i += ip_size;
        }
       
        // DNS
        if(opt_type == OPT_DNS){
            ip_size = dhcp->options[i++];
            switch(ip_size){
                case 4:
                    fillIP((unsigned char*)&dns_ip, (unsigned char*)&dhcp->options[i]);
                    break;  
                default:
                    printf("No IPv4 is %d\n", ip_size);
                    break;
            }
            i += ip_size;
        }
    
        // DHCP
        if(opt_type == OPT_SERVER_ID){
            ip_size = dhcp->options[i++];
            switch(ip_size){
                case 4:
                    fillIP((unsigned char*)&dhcp_ip, (unsigned char*)&dhcp->options[i]);
                    break;  
                default:
                    printf("No IPv4 is %d\n", ip_size);
                    break;
            }
            i += ip_size;
        }
    }

    return 0;
}
