#ifndef __hda_h__
#define __hda_h__
#include <stdint.h>
// GCTL – Global Control
#define  HDA_GCTL_UNSOL  0x100
#define  HDA_GCTL_FCNTRL 0x2
#define  HDA_GCTL_CRST 	 0x1
// GSTS – Global Status
#define HDA_GSTS_FSTS 0x2
// INT Interrupt Control
#define HDA_INT_GIE 0x80000000
#define HDA_INT_CIE 0x40000000
#define HDA_INT_SIE(stream)(stream &0x3FFFFFFF)
// CRORB
#define HDA_CORB_RPST 0x8000
#define HDA_CORB_MEIE 0x1
#define HDA_CORB_DMAE 0x2
// Immediate Command
#define HDA_ICB 0x1
#define HDA_IRV 0x2

//NODE ID
#define NODE_ROOT 0x00
#define NODE_FUNTION 0x1
#define NODE_WIDGET 0x2

#define __codec(cdata,verb,nid,i,cad)\
((cdata &0xff)|((verb &0xfff) <<8 )|((nid &0x7f)<<20)|((i &0x1)<<27)|((cad &0xf)<<28))

typedef volatile struct HDA {
	uint16_t gcap;		// 0x00	Global Capabilities
	uint8_t  vmin;		// 0x02	Minor Version
	uint8_t  vmaj;		// 0x03	Major Version
	uint16_t outpay;	// 0x04	Output Payload Capability 
	uint16_t inpay;		// 0x06	Input Payload Capability 
	uint32_t gctl;		// 0x08	Global Control
	uint16_t wakeen;	// 0x0C	Wake Enable 
	uint16_t statests;	// 0x0E	State Change Status (Wake Status)
	uint16_t gsts;		// 0x10 Global Status 
	uint8_t  rsv0[2];	// 0x12 Reserved 
	uint32_t ecap;		// 0x14 Extended Capabilities (Mobile Only)
	uint16_t outstrmpay;	// 0x18 Output Stream Payload Capability 
	uint16_t instrmpay;	// 0x1A Input Stream Payload Capability
	uint8_t  rsv1[4];	// 0x1C Reserved
	uint32_t intctl;	// 0x20 Interrupt Control 
	uint32_t intsts;	// 0x24 Interrupt Status
	uint8_t  rsv2[8];	// 0x28 Reserved
	uint32_t walclk;	// 0x30 Wall Clock Counter
	uint32_t ssync;		// 0x34 Stream Synchronization TODO The Spec HD Audio coloca este membro no offset 0x38
	uint8_t  rsv3[4];	// 0x38 Reserved 
	uint8_t  rsv4[4];	// 0x3C Reserved
	uint32_t corblbase;	// 0x40 CORB Lower Base Address 
	uint32_t corbubase;	// 0x44 CORB Upper Base Address 
	uint16_t corbwp;	// 0x48 CORB Write Pointer 
	uint16_t corbrp;	// 0x4A CORB Read Pointer
	uint8_t  corbctl;	// 0x4C CORB Control
	uint8_t  corbsts;	// 0x4D CORB Status
	uint8_t  corbsize;	// 0x4E CORB Size
	uint8_t  rsv5[1];	// 0x4F Reserved
	uint32_t rirblbase;	// 0x50 RIRB Lower Base Address 
	uint32_t rirbubase;	// 0x54 RIRB Upper Base Address 
	uint16_t rirbwp;	// 0x58 RIRB Write Pointer
	uint16_t rintctl;	// 0x5A Response Interrupt Count
	uint8_t  rirbctl;	// 0x5C RIRB Control
	uint8_t  rirbsts;	// 0x5D RIRB Status
	uint8_t  rirbsize;	// 0x5E RIRB Size
	uint8_t  rsv6[1];	// 0x5F Reserved 
	uint32_t icoi;		// 0x60 Immediate Command Output Interface
	uint32_t icii;		// 0x64 Immediate Command Input Interface
	uint16_t icis;		// 0x68 Immediate Command Status
	uint8_t  rsv7[6];	// 0x6A Reserved
	uint32_t dplbase;	// 0x70 DMA Position Lower Base Address 
	uint32_t dpubase;	// 0x74 DMA Position Upper Base Address
	uint8_t  rsv8[8];	// 0x78 Reserved
	uint32_t isd0ctl:24;	// 0x80 Input Stream Descriptor 0 (ISD0) Control 
 	uint32_t isd0sts:8;	// 0x83 ISD0 Status
	uint32_t isd0lpib;	// 0x84 ISD0 Link Position in Buffer 
	uint32_t isd0cbl;	// 0x88 ISD0 Cyclic Buffer Length
	uint16_t isd0lvi;	// 0x8C ISD0 Last Valid Index
	uint16_t isd0fifow;	// 0x8E ISD0 FIFO Watermark // Reserved na Spec original
	uint16_t isd0fifos;	// 0x90 ISD0 FIFO Size 
	uint16_t isd0fmt;	// 0x92 ISD0 Format
	uint8_t  rsv10[4];	// 0x94 Reserved
	uint32_t isd0bdpl;	// 0x98 ISD0 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t isd0bdpu;	// 0x9C ISD0 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t isd1ctl:24;	// 0xA0 Input Stream Descriptor 1 (ISD1) Control 
 	uint32_t isd1sts:8;	// 0xA3 ISD1 Status
	uint32_t isd1lpib;	// 0xA4 ISD1 Link Position in Buffer 
	uint32_t isd1cbl;	// 0xA8 ISD1 Cyclic Buffer Length
	uint16_t isd1lvi;	// 0xAC ISD1 Last Valid Index
	uint16_t isd1fifow;	// 0xAE ISD1 FIFO Watermark // Reserved na Spec original
	uint16_t isd1fifos;	// 0xB0 ISD1 FIFO Size 
	uint16_t isd1fmt;	// 0xB2 ISD1 Format
	uint8_t  rsv11[4];	// 0xB4 Reserved
	uint32_t isd1bdpl;	// 0xB8 ISD1 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t isd1bdpu;	// 0xBC ISD1 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t isd2ctl:24;	// 0xC0 Input Stream Descriptor 2 (ISD2) Control 
 	uint32_t isd2sts:8;	// 0xC3 ISD2 Status
	uint32_t isd2lpib;	// 0xC4 ISD2 Link Position in Buffer 
	uint32_t isd2cbl;	// 0xC8 ISD2 Cyclic Buffer Length
	uint16_t isd2lvi;	// 0xCC ISD2 Last Valid Index
	uint16_t isd2fifow;	// 0xCE ISD2 FIFO Watermark // Reserved na Spec original
	uint16_t isd2fifos;	// 0xD0 ISD2 FIFO Size 
	uint16_t isd2fmt;	// 0xD2 ISD2 Format
	uint8_t  rsv12[4];	// 0xD4 Reserved
	uint32_t isd2bdpl;	// 0xD8 ISD2 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t isd2bdpu;	// 0xDC ISD2 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t isd3ctl:24;	// 0xE0 Input Stream Descriptor 3 (ISD3) Control 
 	uint32_t isd3sts:8;	// 0xE3 ISD3 Status
	uint32_t isd3lpib;	// 0xE4 ISD3 Link Position in Buffer 
	uint32_t isd3cbl;	// 0xE8 ISD3 Cyclic Buffer Length
	uint16_t isd3lvi;	// 0xEC ISD3 Last Valid Index
	uint16_t isd3fifow;	// 0xEE ISD3 FIFO Watermark // Reserved na Spec original
	uint16_t isd3fifos;	// 0xF0 ISD3 FIFO Size 
	uint16_t isd3fmt;	// 0xF2 ISD3 Format
	uint8_t  rsv13[4];	// 0xF4 Reserved
	uint32_t isd3bdpl;	// 0xF8 ISD3 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t isd3bdpu;	// 0xFC ISD3 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t osd0ctl:24;// 0x100 Output Stream Descriptor 0 (OSD0) Control 
 	uint32_t osd0sts:8;	// 0x103 OSD0 Status
	uint32_t osd0lpib;	// 0x104 OSD0 Link Position in Buffer 
	uint32_t osd0cbl;	// 0x108 OSD0 Cyclic Buffer Length
	uint16_t osd0lvi;	// 0x10C OSD0 Last Valid Index
	uint16_t osd0fifow;	// 0x10E OSD0 FIFO Watermark // Reserved na Spec original
	uint16_t osd0fifos;	// 0x110 OSD0 FIFO Size 
	uint16_t osd0fmt;	// 0x112 OSD0 Format
	uint8_t  rsv14[4];	// 0x114 Reserved
	uint32_t osd0bdpl;	// 0x118 OSD0 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t osd0bdpu;	// 0x11C OSD0 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t osd1ctl:24;	// 0x120 Onput Stream Descriptor 1 (OSD1) Control 
 	uint32_t osd1sts:8;	// 0x123 OSD1 Status
	uint32_t osd1lpib;	// 0x124 OSD1 Link Position in Buffer 
	uint32_t osd1cbl;	// 0x128 OSD1 Cyclic Buffer Length
	uint16_t osd1lvi;	// 0x12C OSD1 Last Valid Index
	uint16_t osd1fifow;	// 0x12E OSD1 FIFO Watermark // Reserved na Spec original
	uint16_t osd1fifos;	// 0x130 OSD1 FIFO Size 
	uint16_t osd1fmt;	// 0x132 OSD1 Format
	uint8_t  rsv15[4];	// 0x134 Reserved
	uint32_t osd1bdpl;	// 0x138 OSD1 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t osd1bdpu;	// 0x13C OSD1 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t osd2ctl:24;	// 0x140 Output Stream Descriptor 2 (OSD2) Control 
 	uint32_t osd2sts:8;	// 0x143 OSD2 Status
	uint32_t osd2lpib;	// 0x144 OSD2 Link Position in Buffer 
	uint32_t osd2cbl;	// 0x148 OSD2 Cyclic Buffer Length
	uint16_t osd2lvi;	// 0x14C OSD2 Last Valid Index
	uint16_t osd2fifow;	// 0x14E OSD2 FIFO Watermark // Reserved na Spec original
	uint16_t osd2fifos;	// 0x150 OSD2 FIFO Size 
	uint16_t osd2fmt;	// 0x152 OSD2 Format
	uint8_t  rsv16[4];	// 0x154 Reserved
	uint32_t osd2bdpl;	// 0x158 OSD2 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t osd2bdpu;	// 0x15C OSD2 Buffer Descriptor List Pointer-Upper Base Address
	uint32_t osd3ctl:24;	// 0x160 Output Stream Descriptor 3 (OSD3) Control 
 	uint32_t osd3sts:8;	// 0x163 OSD3 Status
	uint32_t osd3lpib;	// 0x164 OSD3 Link Position in Buffer 
	uint32_t osd3cbl;	// 0x168 OSD3 Cyclic Buffer Length
	uint16_t osd3lvi;	// 0x16C OSD3 Last Valid Index
	uint16_t osd3fifow;	// 0x16E OSD3 FIFO Watermark // Reserved na Spec original
	uint16_t osd3fifos;	// 0x170 OSD3 FIFO Size 
	uint16_t osd3fmt;	// 0x172 OSD3 Format
	uint8_t  rsv17[4];	// 0x174 Reserved
	uint32_t osd3bdpl;	// 0x178 OSD3 Buffer Descriptor List Pointer-Lower Base Address 
	uint32_t osd3bdpu;	// 0x17C OSD3 Buffer Descriptor List Pointer-Upper Base Address     	  
}__attribute__((packed)) HDA_T;

//Codec Verb and Response Structures
 
typedef volatile struct VERB {
	uint32_t cmd_data:8;
	uint32_t verb:12;
	uint32_t node_id:8;
	uint32_t codec_addr:4;

}__attribute__((packed)) VERB_T;


typedef struct _HDA_RESPONSE
{
	unsigned int response;
	unsigned int rsv:2;
	unsigned int unsol:1;
	unsigned int valid:1;
	unsigned int nul:28;
}__attribute__((packed)) HDA_RESPONSE;




struct hda_node {
	uint8_t  nid; // verb 0xf00 fun 0x4 
	uint8_t  addr; 
	uint8_t  startn; // verb 0xf00 fun 0x4
	uint8_t  numn; // verb 0xf00 fun 0x4
	uint8_t  type; 
	uint8_t  rsv0[3];
	uint32_t fun_grup_typ; // verb 0xf00 fun 0x5
	uint32_t widgetcap; // verb 0xf00 fun 0x9
	uint32_t pincap;  // verb 0xf00 fun 0xc
	uint32_t inputaplcap; // verb 0xf00 fun 0xd
	uint32_t outputaplcap; // verb 0xf00 fun 0x12
	uint32_t conectlistlen; // verb 0xf00 fun 0xe
	uint32_t volumecap; // verb 0xf00 fun 0x13
	uint32_t defaultconect; // verb 0xF1C
	uint32_t firstentry;  //verb 0xF02
	struct hda_node *next;
}__attribute__((packed)); typedef struct hda_node hda_node_t; 


struct _HDA_ROOT_NODE
{
	int start;
	int num;
}__attribute__((packed));

struct _HDA_AFG_NODE
{
	int start;
	int num;
}__attribute__((packed));

struct _HDA_OUTPUT_NODE
{
	int start;
	int num;
	
}__attribute__((packed));

#endif

