#ifndef __PCNET_H__
#define __PCNET_H__

#define PCNET_TX_BUF_NUM		8		// Tx buffer number
#define PCNET_RX_BUF_NUM		32		// Rx buffer number

enum {
	REG_RDP 	= 0x10,
	REG_RAP 	= 0x12,
	REG_RESET	= 0x14,
	REG_BDP 	= 0x16,
};

#define CSR_STATUS_INIT	(1<< 0)
#define CSR_STATUS_STRT	(1<< 1)
#define CSR_STATUS_IENA	(1<< 6)
#define CSR_STATUS_INTR	(1<< 7)
#define CSR_STATUS_IDON	(1<< 8)
#define CSR_STATUS_TINT	(1<< 9)
#define CSR_STATUS_RINT	(1<<10)

struct TxDesc
{
    unsigned int buffer;
	unsigned short length;
	unsigned short status;
	unsigned int misc;
	unsigned int reserved;
};

struct RxDesc
{
    unsigned int buffer;
	unsigned short buf_length;
	unsigned short status;
	unsigned int msg_length;
	unsigned int reserved;
};

struct pcnet_block
{
	unsigned int mode;	        // [0:15]: MODE, [20:23] RLen, [28:31] TLen
	unsigned int phys_addr0;	// MAC Address
	unsigned int phys_addr1;	// [0:15] MAC Addres
	unsigned int logical_addr_f0;	    // Logical Address Filter (CRC on multicast MAC, top 6 bits index bitmap)
	unsigned int logical_addr_f1;
	unsigned int rx_buffer;	    // Rx Descriptor Ring Address
	unsigned int tx_buffer;	    // Tx Descriptor Ring Address
};


struct pcnet_softc
{
    int type;
    unsigned short vid;
    unsigned short did;
    int intr;
    unsigned long io_base;
    unsigned long base_address;
    // 8, 8, 256*8, 256*8 = 4112KiB/4 = 1028 Blocks
    unsigned long pcnet_block_vmem;
    unsigned long pcnet_block_phymem;
    unsigned long vmem;
    unsigned long phymem;

    unsigned long tx_buf; // physical memory
    unsigned long rx_buf; // physical memory
    unsigned long tx_buf_v; // virtual memory
    unsigned long rx_buf_v; // virtual memory
    int tx_cur;
    int rx_cur;
    int tx_buf_num;
    int rx_buf_num;

    struct TxDesc *tx_desc[PCNET_TX_BUF_NUM];
    struct RxDesc *rx_desc[PCNET_RX_BUF_NUM];

    int chipset;

}__attribute__((packed));

#endif
