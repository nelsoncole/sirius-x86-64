/*
 * File Name: ahci.h
 *
 *
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, nelsoncole
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __AHCI_H__
#define __AHCI_H__

#define 	FIS_TYPE_REG_H2D    	0x27	// Register FIS - host to device
#define 	FIS_TYPE_REG_D2H	0x34	// Register FIS - device to host
#define 	FIS_TYPE_DMA_ACT	0x39	// DMA activate FIS - device to host
#define 	FIS_TYPE_DMA_SETUP	0x41	// DMA setup FIS - bidirectional
#define 	FIS_TYPE_DATA		0x46	// Data FIS - bidirectional
#define 	FIS_TYPE_BIST		0x58	// BIST activate FIS - bidirectional
#define 	FIS_TYPE_PIO_SETUP	0x5F	// PIO setup FIS - device to host
#define 	FIS_TYPE_DEV_BITS	0xA1	// Set device bits FIS - device to host


#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

 
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

// PxCMD - Port x Command and Status
#define HBA_PxCMD_ST    0x1     // Start (ST)
#define HBA_PxCMD_FRE   0x10    // FIS Receive Enable (FRE)
#define HBA_PxCMD_FR    0x4000  // FIS Receive Running (FR)
#define HBA_PxCMD_CR    0x8000  // Command List Running (CR)
#define HBA_PxIS_TFES   0x40000000

// HBA MEMORY PORT
typedef volatile struct HBA_PORT {
    	uint32_t clb;		// 0x00, Port x Command List Base Address
	uint32_t clbu;		// 0x04, Port x Command List Base Address Upper 32-Bits
	uint32_t fb;		// 0x08, Port x FIS Base Address
	uint32_t fbu;		// 0x0C, Port x FIS base address upper 32 bits
	uint32_t is;		// 0x10, Port x Interrupt status
	uint32_t ie;		// 0x14, Port x Interrupt enable
	uint32_t cmd;		// 0x18, Port x Command and status
	uint32_t rsv0;		// 0x1C, Port x Reserved
	uint32_t tfd;		// 0x20, Port x Task File Data
	uint32_t sig;		// 0x24, Port x Signature
	uint32_t ssts;		// 0x28, Port x SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, Port x SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, Port x SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, Port x SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, Port x Command issue
	uint32_t sntf;		// 0x3C, Port x SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, Port x FIS-based switch control
    	uint32_t devslp;    	// 0x44, Port x Device Sleep
	uint32_t rsv1[10];	// 0x48 ~ 0x6F, Port x Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, Port x vendor specific
}HBA_PORT_T;

// HBA MEMORY SPACE
typedef volatile struct HBA_MEM {
    	// Generic Host Control (00h - 2Bh)
    	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;    	// 0x1C, Enclosure management location
	uint32_t em_ctl;	// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status
    	// Reserved (2Ch - 5Fh)
	uint8_t  reserved[0x60-0x2C];	
    	// Reserved for NVMHCI (60h - 9Fh)
    	uint8_t  reserved_for_nvhci[0xA0-0x60];
    	// Vendor Specific registers (A0h - FFh)
    	uint8_t  vendor[0x100-0xA0];
    	// Port 0 port control registers (100h - 17Fh)
    	// Port 1 port control registers (180h - 1FFh)
    	// (Ports 2 – port 29 control registers) (200h - FFFh)
    	// Port 30 port control registers (1000h - 107Fh)
    	// Port 30 port control registers (1080h - 10FFh)
    	HBA_PORT_T ports[32];
}HBA_MEM_T;

struct DMA_SETUP_FIS
{
	// DWORD 0
	uint8_t  fis_type;	    // FIS_TYPE_DMA_SETUP
	uint8_t  pmport:4;	    // Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		    // Data transfer direction, 1 - device to host
	uint8_t  i:1;		    // Interrupt bit
	uint8_t  a:1;           // Auto-activate. Specifies if DMA Activate FIS is needed
    	uint8_t  rsved[2];      // Reserved
 
	//DWORD 1&2
    	uint64_t DMAbufferID;   // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. 
                            // SATA Spec says host    specific and not in Spec. Trying AHCI spec might work.
    	//DWORD 3
    	uint32_t rsvd;           //More reserved
 
    	//DWORD 4
    	uint32_t DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0
 
    	//DWORD 5
    	uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0
 
    	//DWORD 6
    	uint32_t resvd;          //Reserved
 
};

struct PIO_SETUP_FIS{
	// DWORD 0
	uint8_t  fis_type;	    // FIS_TYPE_PIO_SETUP
	uint8_t  pmport:4;	    // Port multiplier
	uint8_t  rsv0:1;		// Reserved
	uint8_t  d:1;		    // Data transfer direction, 1 - device to host
	uint8_t  i:1;		    // Interrupt bit
	uint8_t  rsv1:1;
	uint8_t  status;		// Status register
	uint8_t  error;		    // Error register
 
	// DWORD 1
	uint8_t  lba0;		    // LBA low register, 7:0
	uint8_t  lba1;		    // LBA mid register, 15:8
	uint8_t  lba2;		    // LBA high register, 23:16
	uint8_t  device;		// Device register
 
	// DWORD 2
	uint8_t  lba3;		    // LBA register, 31:24
	uint8_t  lba4;		    // LBA register, 39:32
	uint8_t  lba5;		    // LBA register, 47:40
	uint8_t  rsv2;		    // Reserved
 
	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  rsv3;		    // Reserved
	uint8_t  e_status;	    // New value of status register
 
	// DWORD 4
	uint16_t tc;		    // Transfer count
	uint8_t  rsv4[2];	    // Reserved
};

struct D2H_REGISTER_FIS
{
	// DWORD 0
	uint8_t  fis_type;      // FIS_TYPE_REG_D2H
	uint8_t  pmport:4;      // Port multiplier
	uint8_t  rsv0:2;        // Reserved
	uint8_t  i:1;           // Interrupt bit
	uint8_t  rsv1:1;        // Reserved
	uint8_t  status;        // Status register
	uint8_t  error;         // Error register
 
	// DWORD 1
	uint8_t  lba0;          // LBA low register, 7:0
	uint8_t  lba1;          // LBA mid register, 15:8
	uint8_t  lba2;          // LBA high register, 23:16
	uint8_t  device;           // Device register
 
	// DWORD 2
	uint8_t  lba3;          // LBA register, 31:24
	uint8_t  lba4;          // LBA register, 39:32
	uint8_t  lba5;          // LBA register, 47:40
	uint8_t  rsv2;          // Reserved
 
	// DWORD 3
	uint8_t  countl;        // Count register, 7:0
	uint8_t  counth;        // Count register, 15:8
	uint8_t  rsv3[2];       // Reserved
 
	// DWORD 4
	uint8_t  rsv4[4];       // Reserved
};

typedef struct H2D_REGISTER_FIS
{	
    // DWORD 0
	uint8_t  fis_type;	    // FIS_TYPE_REG_H2D
 
	uint8_t  pmport:4;	    // Port multiplier
	uint8_t  rsv0:3;		// Reserved
	uint8_t  c:1;		    // 1: Command, 0: Control
 
	uint8_t  command;	    // Command register
	uint8_t  featurel;	    // Feature register, 7:0
 
	// DWORD 1
	uint8_t  lba0;		    // LBA low register, 7:0
	uint8_t  lba1;		    // LBA mid register, 15:8
	uint8_t  lba2;		    // LBA high register, 23:16
	uint8_t  device;		    // Device register
 
	// DWORD 2
	uint8_t  lba3;		    // LBA register, 31:24
	uint8_t  lba4;		    // LBA register, 39:32
	uint8_t  lba5;		    // LBA register, 47:40
	uint8_t  featureh;	    // Feature register, 15:8
 
	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;		    // Isochronous command completion
	uint8_t  control;	    // Control register
 
	// DWORD 4
	uint8_t  rsv1[4];	    // Reserved
}H2D_REGISTER_FIS_T;

struct SET_DEV_BITS_FIS{

    // DWORD 0
    uint8_t  fis_type;	    // FIS_TYPE_BIST
    uint8_t  pmport:4;	    // Port multiplier
    uint8_t  rsv0:2;		// Reserved
    uint8_t  i:1;		    // Interrupt Bit
    uint8_t  n:1;		    // Notification bit
    uint8_t  stsl:3;		// Statuslo
    uint8_t  rsv1:1;		// Reserved
    uint8_t  stsh:3;		// StatusHi
    uint8_t  rsv2:1;		// Reserved
    uint8_t  error;         // Error
    // DWORD 1
    uint32_t pspec;         // Protocol Specific

};

typedef struct HBA_ACMD{

    uint8_t packet[12];

}HBA_ACMD_T;


struct DATA_FIS
{
	// DWORD 0
	uint8_t  fis_type;	    // FIS_TYPE_DATA
 
	uint8_t  pmport:4;	    // Port multiplier
	uint8_t  rsv0:4;		// Reserved
 
	uint8_t  rsv1[2];	    // Reserved
 
	// DWORD 1 ~ N
	uint32_t data[1];	    // Payload
};


// Received FIS Structure (Port x FBU or Port x FB)
typedef volatile struct HBA_FIS {
    // DSFIS: DMA Setup FIS (00h - 1Ch)
    struct DMA_SETUP_FIS dsfis;
    // reserved
    uint8_t pad0[4];
    // PSFIS: PIO Setup FIS (20h - 34h)
    struct PIO_SETUP_FIS psfis;
    // reserved
    uint8_t pad1[12];
    // RFIS: D2H Register FIS (40h - 54h)
    struct D2H_REGISTER_FIS rfis;
    // reserved 
    uint8_t pad2[4];
    // SDBFIS: Set Device Bits FIS (58h - 5Fh)
    struct SET_DEV_BITS_FIS sdbfis;
    // UFIS: Unknown FIS (up to 64 bytes) (60h - A0h)
    uint8_t ufis[64];
    // reserved (A0h - FFh)
    uint8_t rsv[0x100-0xA0];  

}HBA_FIS_T;


//  Command List Structure

// command header 0
typedef struct HBA_CMD_HEADER
{
	// DW0
	uint8_t  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	uint8_t  a:1;		// ATAPI
	uint8_t  w:1;		// Write, 1: H2D, 0: D2H
	uint8_t  p:1;		// Prefetchable
 
	uint8_t  r:1;		// Reset
	uint8_t  b:1;		// BIST
	uint8_t  c:1;		// Clear busy upon R_OK
	uint8_t  rsv0:1;		// Reserved
	uint8_t  pmp:4;		// Port multiplier port
 
	uint16_t prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	uint32_t prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	uint32_t rsv1[4];	// Reserved
} HBA_CMD_HEADER_T;


// Physical Region Descriptor Table (PRDT)
typedef struct HBA_PRDT_ENTRY
{
	uint32_t dba;		// Data base address
	uint32_t dbau;		// Data base address upper 32 bits
	uint32_t rsv0;		// Reserved
 
	// DW3
	uint32_t dbc:22;		// Byte count, 4M max
	uint32_t rsv1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion
} HBA_PRDT_ENTRY_T;


// Command Table
typedef struct HBA_CMD_TBL
{
	// 0x00
	uint8_t  cfis[64];	// Command FIS
 
	// 0x40
	uint8_t  acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	uint8_t  rsv[48];	// Reserved
 
	// 0x80
	HBA_PRDT_ENTRY_T prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL_T;

int sata_read_sector(int satanum,uint64_t lba, uint32_t count,void *buf);
int sata_write_sector(int satanum,uint64_t lba, uint32_t count,void *buf);


#endif
