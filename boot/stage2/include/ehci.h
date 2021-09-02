#ifndef __EHCI_H
#define __EHCI_H

// 0x8086 0x24DD (USB EHCI—D29:F7) ICH5 


typedef volatile struct EHCI_PORT
{
	unsigned int ccsts:1;       // Current Connect Status
    	unsigned int cstsc:1;       // Connect Status Change
   	unsigned int ped:1;         // Port Enabled/Disabled
    	unsigned int pdc:1;         // Port Enable/Disable Change 
    	unsigned int overa:1;       // Overcurrent Active
   	unsigned int overc:1;       // Overcurrent Change 
 	unsigned int fpresume:1;    // Force Port Resume
	unsigned int Suspend:1;     // Suspend 
    	unsigned int preset:1;      // Port Reset
    	unsigned int rsv0:1;
    	unsigned int lsts:2;        // Line Status
    	unsigned int pp:1;          // Port Power
    	unsigned int po:1;          // Port Owner 
   	unsigned int rsv1:2;
    	unsigned int ptc:4;         // Port Test Control 
    	unsigned int wce:1;         // Wake on Connect Enable (WKCNNT_E)
    	unsigned int wde:1;         // Wake on Disconnect Enable (WKDSCNNT_E)
    	unsigned int woe:1;         // Wake on Overcurrent Enable (WKOC_E)
    	unsigned int rsv2:9;

}__attribute__((packed)) EHCI_PORT_T;

typedef volatile struct EHCI_MEM
{
    	unsigned short caplength;     // Capabilities Registers Length
    	unsigned short hcivrsion;     // Host Controller Interface Version Number
   	unsigned int hcsparams;     // Host Controller Structural Parameters, value default(00103206h(D29:F7), 00102205(D26:F7))
    	unsigned int hccparams;     // Host Controller Capability Parameters
    	unsigned char  rsv0[0x20-0x0C];
    	// +offset 20h
  	unsigned int cmd;           // USB2.0_CMD USB 2.0 Command 
    	unsigned int sts;           // USB2.0_STS USB 2.0 Status
    	unsigned int intr;          // USB 2.0_INTR USB 2.0 Interrupt Enable 
    	unsigned int frindex;       // USB 2.0 Frame Index
    	unsigned int ctrldssegment; // Control Data Structure Segment 
    	unsigned int perodiclistbase; // Period Frame List Base Address 
    	unsigned int asynclistaddr; // Current Asynchronous List Address 
    	unsigned char  rsv1[0x60-0x3C]; // Reserved
    	unsigned int configflag;    // Configure Flag
    	EHCI_PORT_T port[6];    // Port x Status and Control TODO Port6 74h–77h (D29 Only), Port7 78h–7Bh (D29 Only)


    	// Debug Port Register Address Map
    	unsigned char  rsv2[0xA0-0x7C];
    	unsigned int ctrlsts;       // Control/Status 
    	unsigned int pid;           // USB PIDs
    	unsigned int datal;       // Data Buffer (Bytes 7:0)
    	unsigned int datah;
    	unsigned int config;        // config   
    	unsigned char  rsv3[0x400-0xB4]; 
}__attribute__((packed)) EHCI_MEM_T;



typedef struct _ehci {
	unsigned int vid;
	unsigned int did;
	unsigned int mmio_base;
}__attribute__((packed)) ehci_t;

typedef struct  {
    unsigned char bRequestType;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} EhciCMD;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bEndpointAddress;
	unsigned char bmAttributes;
	unsigned short wMaxPacketSize;
	unsigned char bInterval;
}EHCI_DEVICE_ENDPOINT;


#define EHCI_PERIODIC_FRAME_SIZE    1024


#define MAX_QH                          8
#define MAX_TD                          32

typedef volatile struct _EHCI_TD
{
	unsigned int nextlink;
    	unsigned int altlink;
    	unsigned int token;
    	unsigned int buffer[5];
    	unsigned int extbuffer[5];
    	

}__attribute__((packed)) EHCI_TD;

typedef volatile struct _EHCI_QH
{
    	unsigned int qhlp;       // Queue Head Horizontal Link Pointer
    	unsigned int ch;         // Endpoint Characteristics
    	unsigned int caps;       // Endpoint Capabilities
    	unsigned int curlink;

    	// matches a transfer descriptor
    	unsigned int nextlink;
    	unsigned int altlink;
    	unsigned int token;
    	unsigned int buffer[5];
    	unsigned int extbuffer[5];

} __attribute__((packed)) EHCI_QH;


//
//
struct cdbres_inquiry {
    unsigned char pdt;
    unsigned char removable;
    unsigned char reserved_02[2];
    unsigned char additional;
    unsigned char reserved_05[3];
    char vendor[8];
    char product[16];
    char rev[4];
} __attribute__ ((packed));

struct cbw_t {
	unsigned long sig;
	unsigned long tag;
	unsigned long xfer_len;
	unsigned char flags;
	unsigned char lun;
	unsigned char wcb_len;
	unsigned char cmd[16];
}  __attribute__ ((packed));

typedef struct __attribute__ ((packed)) {
	unsigned long signature;
	unsigned long tag;
	unsigned long dataResidue;
	unsigned char status;
}CommandStatusWrapper;

struct rdcap_10_response_t {
	unsigned long max_lba;
	unsigned long blk_size;
} __attribute__ ((packed));

typedef struct {
	unsigned char key;
	unsigned char code;
	unsigned char qualifier;
}SCSIStatus;

#define USB_STORAGE_ENABLE_ENQ 1
#define USB_STORAGE_ENABLE_CAP 0
#define USB_STORAGE_ENABLE_SEC 1
#define USB_STORAGE_SECTOR_SIZE 512
#define USB_STORAGE_CSW_SIGN 0x53425355


void ehci_asynchronous_schedule_enable (EHCI_MEM_T *mmio);
void ehci_asynchronous_schedule_disable(EHCI_MEM_T *mmio);

int ehci_init(void);


void ehci_int_td( EHCI_TD *td, EHCI_TD *nextlink, unsigned int toggle, unsigned int packettype,
unsigned int len, const void *data);

int ehci_wait_for_completion(EHCI_TD *td);
void ehci_init_qh(EHCI_QH *sqh1, EHCI_QH *sqh2, EHCI_QH *curlink, EHCI_TD *std, int interrupt, 
unsigned int speed,unsigned int addr, unsigned int endp, unsigned int maxSize);

int usb_hub_init();

struct usb_hub_desc {
	unsigned char length;
	unsigned char type;
	unsigned char portcnt;
	unsigned short characteristics;
	unsigned char pwdgood;
	unsigned char current;
	unsigned char variable[2];

}__attribute__ ((packed));

struct usb_device_desc {
    	unsigned char length;
	unsigned char type;
    	unsigned short bcdusb;
    	unsigned char class;
   	unsigned char subclass;
    	unsigned char protocol;
    	unsigned char maxpacketsize;
   	unsigned short vendorid;
    	unsigned short productid;
    	unsigned short devicever;
    	unsigned char vendorstr;
    	unsigned char productstr;
    	unsigned char serialstr;
   	unsigned char confcount;
} __attribute__ ((packed));

int usb_control_msg (EHCI_MEM_T *controller, int port, 
			int bRequestType,
                     	int bRequest,
			int wValue,
    			int wIndex, int wLength, void *data);
    			



#endif
