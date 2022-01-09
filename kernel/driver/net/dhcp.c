#include <ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sleep.h>

#include "udp.h"
#include "dhcp.h"


int dhcp_send(unsigned char message_type)
{

    unsigned char yourIP[4] = {192,168,43,251};
    unsigned char serverID[4] = {192,168,43,1};
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
            dhcp->options[5] = yourIP[0];
            dhcp->options[6] = yourIP[1];
            dhcp->options[7] = yourIP[2];
            dhcp->options[8] = yourIP[3];
            // Server Identifier
            dhcp->options[9] = OPT_SERVER_ID;
            dhcp->options[10]= SIZE_OF_IP;
            dhcp->options[11]= serverID[0];
            dhcp->options[12]= serverID[1];
            dhcp->options[13]= serverID[2];
            dhcp->options[14]= serverID[3];
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
