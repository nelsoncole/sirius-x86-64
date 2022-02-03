#ifndef __RTL_H__
#define __RTL_H__



#define RE_TX_BUF_NUM		256		// Tx buffer number
#define RE_RX_BUF_NUM		256		// Rx buffer number

// RealTek PCI vendor ID
#define	RE_VENDORID 0x10EC
// D-Link vendor ID
#define DLINK_VENDORID  0x1186


#define	RL_ADDR_LO(y)		((unsigned long long) (y) & 0xFFFFFFFF)
#define	RL_ADDR_HI(y)		((unsigned long long) (y) >> 32)

/*
 * Command register.
 */
#define RE_CMD_EMPTY_RXBUF	0x0001
#define RE_CMD_TX_ENB		0x0004
#define RE_CMD_RX_ENB		0x0008
#define RE_CMD_RESET		0x0010


#define Jumbo_Frame_2k	(2 * 1024)
#define Jumbo_Frame_3k	(3 * 1024)
#define Jumbo_Frame_4k	(4 * 1024)
#define Jumbo_Frame_5k	(5 * 1024)
#define Jumbo_Frame_6k	(6 * 1024)
#define Jumbo_Frame_7k	(7 * 1024)
#define Jumbo_Frame_8k	(8 * 1024)
#define Jumbo_Frame_9k	(9 * 1024)

enum {
        MACFG_3 = 0x03,
        MACFG_4,
        MACFG_5,
        MACFG_6,

        MACFG_11,
        MACFG_12,
        MACFG_13,
        MACFG_14,
        MACFG_15,
        MACFG_16,
        MACFG_17,
        MACFG_18,
        MACFG_19,

        MACFG_21,
        MACFG_22,
        MACFG_23,
        MACFG_24,
        MACFG_25,
        MACFG_26,
        MACFG_27,
        MACFG_28,

        MACFG_31,
        MACFG_32,
        MACFG_33,

        MACFG_36,
        MACFG_37,
        MACFG_38,
        MACFG_39,

        MACFG_41,
        MACFG_42,
        MACFG_43,

        MACFG_50,
        MACFG_51,
        MACFG_52,
        MACFG_53,
        MACFG_54,
        MACFG_55,
        MACFG_56,
        MACFG_57,
        MACFG_58,
        MACFG_59,
        MACFG_60,
        MACFG_61,
        MACFG_62,
        MACFG_63,
        MACFG_64,
        MACFG_65,
        MACFG_66,
        MACFG_67,
        MACFG_68,
        MACFG_69,

        MACFG_FF = 0xFF
};


struct RxDesc
{
    unsigned int Frame_Length:14;
    unsigned int TCPF:1;
    unsigned int UDPF:1;
    unsigned int IPF:1;
    unsigned int TCPT:1;
    unsigned int UDPT:1;
    unsigned int CRC:1;
    unsigned int RUNT:1;
    unsigned int RES:1;
    unsigned int RWT:1;
    unsigned int RESV:2;
    unsigned int BAR:1;
    unsigned int PAM:1;
    unsigned int MAR:1;
    unsigned int LS:1;
    unsigned int FS:1;
    unsigned int EOR:1;
    unsigned int OWN:1;

    unsigned int VLAN_TAG:16;
    unsigned int TAVA:1;
    unsigned int RESV1:15;

    unsigned int RxBuffL;
    unsigned int RxBuffH;

}__attribute__((packed));

struct TxDesc
{
    unsigned int Frame_Length:16;
    unsigned int TCPCS:1;
    unsigned int UDPCS:1;
    unsigned int IPCS:1;
    unsigned int SCRC:1;
    unsigned int RESV:6;
    unsigned int TDMA:1;
    unsigned int LGSEN:1;
    unsigned int LS:1;
    unsigned int FS:1;
    unsigned int EOR:1;
    unsigned int OWN:1;

    unsigned int VLAN_TAG:16;
    unsigned int TAGC0:1;
    unsigned int TAGC1:1;
    unsigned int RESV1:14;

    unsigned int TxBuffL;
    unsigned int TxBuffH;

}__attribute__((packed));

struct re_softc
{
    int type;
    unsigned short vid;
    unsigned short did;
    int intr;
    unsigned long base_address;
    // 8, 8, 256*8, 256*8 = 4112KiB/4 = 1028 Blocks
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

    struct TxDesc *tx_desc[RE_TX_BUF_NUM];
    struct RxDesc *rx_desc[RE_RX_BUF_NUM];

    int max_jumbo_frame_size;
    int chipset;

}__attribute__((packed));



#endif
