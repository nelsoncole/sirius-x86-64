#ifndef __SOCKET_H
#define __SOCKET_H

/*
       SOCK_STREAM     Provides sequenced, reliable, two-way, connection-
                       based byte streams.  An out-of-band data transmission
                       mechanism may be supported.

       SOCK_DGRAM      Supports datagrams (connectionless, unreliable
                       messages of a fixed maximum length).

       SOCK_SEQPACKET  Provides a sequenced, reliable, two-way connection-
                       based data transmission path for datagrams of fixed
                       maximum length; a consumer is required to read an
                       entire packet with each input system call.

       SOCK_RAW        Provides raw network protocol access.

       SOCK_RDM        Provides a reliable datagram layer that does not
                       guarantee ordering.

       SOCK_PACKET     Obsolete and should not be used in new programs;
*/

#define SOCK_STREAM       	1     /* stream (connection) socket	*/
#define SOCK_DGRAM        	2     /* datagram (conn.less) socket	*/
#define SOCK_RAW          	3     /* raw socket			        */
#define SOCK_RDM          	4     /* reliably-delivered message	*/
#define SOCK_SEQPACKET    	5     /* sequential packet socket	*/ /* sequenced packet stream */
#define SOCK_DCCP  		6
// ...
#define SOCK_PACKET       	10    /* linux specified 		    */
#define SOCK_MAX            	(128)


// Supported address families.

#define AF_UNSPEC     		0
#define AF_UNIX       		1     /* Unix domain sockets 		*/
#define AF_LOCAL      		1     /* POSIX name for AF_UNIX	*/
#define AF_INET       		2     /* Internet IP Protocol 	*/
#define AF_AX25       		3     /* Amateur Radio AX.25 		*/
#define AF_IPX        		4     /* Novell IPX 			*/
#define AF_APPLETALK  		5     /* AppleTalk DDP 		*/
#define AF_NETROM     		6     /* Amateur Radio NET/ROM 	*/
#define AF_BRIDGE     		7     /* Multiprotocol bridge 	*/
#define AF_ATMPVC     		8     /* ATM PVCs			*/
#define AF_X25        		9     /* Reserved for X.25 project 	*/
#define AF_INET6      		10    /* IP version 6			*/
#define AF_ROSE       		11    /* Amateur Radio X.25 PLP	*/
#define AF_DECnet     		12    /* Reserved for DECnet project	*/
#define AF_NETBEUI    		13    /* Reserved for 802.2LLC project*/
#define AF_SECURITY   		14    /* Security callback pseudo AF */
#define AF_KEY        		15    /* PF_KEY key management API */
#define AF_NETLINK    		16
#define AF_ROUTE      		AF_NETLINK    /* Alias to emulate 4.4BSD */
#define AF_PACKET     		17            /* Packet family		*/
#define AF_ASH        		18            /* Ash				*/
#define AF_ECONET     		19            /* Acorn Econet			*/
#define AF_ATMSVC     		20            /* ATM SVCs			*/
#define AF_SNA        		22            /* Linux SNA Project (nutters!) */
#define AF_IRDA       		23            /* IRDA sockets			*/
#define AF_PPPOX      		24            /* PPPoX sockets		*/
#define AF_WANPIPE    		25            /* Wanpipe API Sockets */
#define AF_LLC        		26            /* Linux LLC			*/
#define AF_TIPC       		30            /* TIPC sockets			*/
#define AF_BLUETOOTH  		31            /* Bluetooth sockets 		*/
#define AF_IUCV       		32            /* IUCV sockets			*/
#define AF_RXRPC      		33            /* RxRPC sockets 		*/
#define AF_RS232      		35            /* Serial socket (NEW!) */
#define AF_MAX        		45

/* Flags we can use with send/ and recv. 
   Added those for 1003.1g not all are supported yet */
 
#define MSG_OOB        1
#define MSG_PEEK       2
#define MSG_DONTROUTE  4
#define MSG_TRYHARD    4     /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC     8
#define MSG_PROBE      0x10  /* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC      0x20
#define MSG_DONTWAIT   0x40    /* Nonblocking io		 */
#define MSG_EOR        0x80    /* End of record */
#define MSG_WAITALL    0x100   /* Wait for a full request */
#define MSG_FIN        0x200
#define MSG_SYN        0x400
#define MSG_CONFIRM    0x800   /* Confirm path validity */
#define MSG_RST        0x1000
#define MSG_ERRQUEUE   0x2000  /* Fetch message from error queue */
#define MSG_NOSIGNAL   0x4000  /* Do not generate SIGPIPE */
#define MSG_MORE       0x8000  /* Sender will send more */
#define MSG_EOF        MSG_FIN





#endif
