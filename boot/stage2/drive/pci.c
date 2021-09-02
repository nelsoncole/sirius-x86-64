#include <pci.h>

const char *pci_classes[]={
    	"Unclassified",
    	"Mass Storage Controller",
    	"Network Controller",
    	"Display Controller",
    	"Multimedia Controller",
    	"Memory Controller",
    	"Bridge device",
    	"Simple Communication Controller",
    	"Base System Peripheral",
    	"Input Device Controller",
    	"Docking Station",
    	"Processor",
    	"Serial Bus Controller",
    	"Wireless Controller",
    	"Intelligent I/O Controllers",
    	"Satellite Communication Controller",
    	"Encryption Controller",
    	"Signal Processing Controller",
    	"Processing Accelerator",
	"Non-Essential Instrumentation",
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//[0x40 - 0x14]="(Reserved)",
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,
	"Co-Processor",
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//[0xFE - 0x41]="(Reserved)",
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	"Unassigned Class (Vendor specific)",
};

//PCI Express, compatibility to PCI local Bus 3.0 
PCI_CLASS_NAME pci_class_names [256] = {
  	{0x000000, 	"Non-VGA-Compatible devices"								},
  	{0x000100, 	"VGA-Compatible Device"									},
  	{0x010000, 	"SCSI bus controller"									},
  	{0x010100,	"IDE controller (ISA Compatibility mode-only controller)"				},
	{0x010105,	"IDE controller (PCI native mode-only controller)"					},
	{0x01010A,	"IDE controller (ISA Compatibility mode controller, \
supports both channels switched to PCI native mode)"								},
	{0x01010F,	"IDE controller (PCI native mode controller, \
supports both channels switched to ISA compatibility mode)"							},
	{0x010180,	"IDE controller (ISA Compatibility mode-only controller, supports bus mastering)"	},
	{0x010185,	"IDE controller (PCI native mode-only controller, supports bus mastering )"		},
	{0x01018A,	"IDE controller (ISA Compatibility mode controller, \
supports both channels switched to PCI native mode, supports bus mastering )"					},
	{0x01018F,	"IDE controller (PCI native mode controller, \
supports both channels switched to ISA compatibility mode, supports bus mastering )"				},
  	{0x010200, 	"Floppy disk controller"								},
  	{0x010300, 	"IPI bus controller"									},
  	{0x010400, 	"RAID bus controller"									},
	{0x010520, 	"ATA Controller (Single DMA)"								},
	{0x010530, 	"ATA Controller (Chained DMA)"								},
	{0x010600, 	"Serial ATA controller (vendor specific interface)"					},
	{0x010601, 	"Serial ATA controller (AHCI 1.0 interface)"						},
	{0x010602, 	"Serial ATA controller (Serial Storage Bus)"						},
	{0x010700, 	"Serial Attached SCSI (SAS)"								},
	{0x010701, 	"Serial Attached SCSI (Serial Storage Bus)"						},
	{0x010801, 	"Non-Volatile Memory Controlle (NVMHCI)"						},
	{0x010802, 	"Non-Volatile Memory Controlle (NVM Express)"						},
	{0x018000, 	"Other mass storage controller"								},
  	{0x020000, 	"Ethernet controller"									},
  	{0x020100, 	"Token ring controller"									},
  	{0x020200, 	"FDDI controller"									},
  	{0x020300, 	"ATM controller"									},
  	{0x020400, 	"ISDN controller"									},
	{0x020500, 	"WorldFip Controller"									},
	{0x020600, 	"PICMG 2.14 Multi Computing"								},
	{0x020700, 	"Infiniband Controller"									},
	{0x020800, 	"Fabric Controller"									},
  	{0x028000, 	"Other Network controller"								},
  	{0x030000, 	"VGA Compatible controller (VGA Controller)"						},
	{0x030001, 	"VGA Compatible controller (8514-Compatible Controller )"				},
  	{0x030100, 	"XGA controller"									},
  	{0x030200, 	"3D controller (Not VGA-Compatible)"							},
  	{0x038000, 	"Other Display controller"								},
  	{0x040000, 	"Multimedia video controller"								},
  	{0x040100, 	"Multimedia audio controller (AC'97)"							},
  	{0x040200, 	"Computer telephony device"								},
	{0x040300, 	"Audio Device (Intel High Definition Audio (HDA) Controller)"				},
  	{0x048000, 	"Other Multimedia controller"								},
  	{0x050000, 	"RAM controller"									},
  	{0x050100, 	"FLASH controller"									},
  	{0x058000, 	"Other Memory controller"								},
  	{0x060000, 	"Host bridge"										},
  	{0x060100, 	"ISA bridge"										},
  	{0x060200, 	"EISA bridge"										},
  	{0x060300, 	"MCA bridge"										},
  	{0x060400, 	"PCI-to-PCI Bridge (Normal Decode)"							},
	{0x060401, 	"PCI-to-PCI Bridge (Subtractive Decode)"						},
  	{0x060500, 	"PCMCIA bridge"										},
  	{0x060600, 	"NuBus bridge"										},
  	{0x060700, 	"CardBus bridge"									},
  	{0x060800, 	"RACEway bridge (Transparent Mode)"							},
	{0x060801, 	"RACEway bridge (Endpoint Mode)"							},
  	{0x060940, 	"PCI-to-PCI Bridge (Semi-Transparent, Primary bus towards host CPU)"			},
	{0x060980, 	"PCI-to-PCI Bridge (Semi-Transparent, Secondary bus towards host CPU)"			},
  	{0x060A00, 	"InfiniBand to PCI host bridge"								},
  	{0x068000, 	"Other Bridge"										},
  	{0x070000, 	"Serial controller (8250-Compatible (Generic XT))"					},
	{0x070001, 	"Serial controller (16450-Compatible )"							},
	{0x070002, 	"Serial controller (16550-Compatible )"							},
	{0x070003, 	"Serial controller (16650-Compatible )"							},
	{0x070004, 	"Serial controller (16750-Compatible)"							},
	{0x070005, 	"Serial controller (16850-Compatible)"							},
	{0x070006, 	"Serial controller (16950-Compatible)"							},
	{0x070100, 	"Parallel Controller (Standard Parallel Port)"						},
	{0x070101, 	"Parallel Controller (Bi-Directional Parallel Port)"					},
	{0x070102, 	"Parallel Controller (ECP 1.X Compliant Parallel Port)"					},
	{0x070103, 	"Parallel Controller (IEEE 1284 Controller)"						},
	{0x0701FE, 	"Parallel Controller (IEEE 1284 Target Device)"						},
	{0x070200, 	"Multiport Serial Controller"								},
	{0x070300, 	"Modem (Generic Modem)"									},
	{0x070301, 	"Modem (Hayes 16450-Compatible Interface)"						},
	{0x070302, 	"Modem (Hayes 16550-Compatible Interface)"						},
	{0x070303, 	"Modem (Hayes 16650-Compatible Interface)"						},
	{0x070304, 	"Modem (Hayes 16750-Compatible Interface)"						},
	{0x070400, 	"GPIB (IEEE 488.1/2) Controller"							},
	{0x070500, 	"Smart Card"										},
	{0x078000, 	"Other Simple Communication controller"							},
  	{0x080000, 	"PIC (Generic 8259-Compatible)"								},
	{0x080001, 	"PIC (ISA-Compatible)"									},
	{0x080002, 	"PIC (EISA-Compatible)"									},
	{0x080010, 	"PIC (I/O APIC Interrupt Controller)"							},
	{0x080020, 	"PIC (I/O(x) APIC Interrupt Controller)"						},
  	{0x080100, 	"DMA controller (Generic 8237-Compatible)"						},
	{0x080101, 	"DMA controller (ISA-Compatible)"							},
	{0x080102, 	"DMA controller (EISA-Compatible)"							},
  	{0x080200, 	"Timer (Generic 8254-Compatible)"							},
	{0x080201, 	"Timer (ISA-Compatible)"								},
	{0x080202, 	"Timer (EISA-Compatible)"								},
	{0x080203, 	"Timer (HPET)"										},
  	{0x080300, 	"RTC (Generic RTC)"									},
	{0x080301, 	"RTC (ISA-Compatible)"									},
  	{0x080400, 	"PCI Hot-plug controller"								},
	{0x080500, 	"SD Host controller"									},
	{0x080600, 	"IOMMU"											},
  	{0x088000, 	"Other System peripheral controller"							},
  	{0x090000, 	"Keyboard controller"									},
  	{0x090100, 	"Digitizer Pen"										},
  	{0x090200, 	"Mouse controller"									},
  	{0x090300, 	"Scanner controller"									},
  	{0x090400, 	"Gameport controller (Generic)"								},
	{0x090401, 	"Gameport controller (Extended)"							},
  	{0x098000, 	"Other input controller"								},
  	{0x0A0000, 	"Generic Docking Station"								},
  	{0x0A8000, 	"Other type of docking station"								},
  	{0x0B0000, 	"386"											},
  	{0x0B0100, 	"486"											},
  	{0x0B0200, 	"Pentium"										},
  	{0x0B1000, 	"Alpha"											},
  	{0x0B2000, 	"Power PC"										},
  	{0x0B3000, 	"MIPS"											},
  	{0x0B4000, 	"Co-processor"										},
  	{0x0C0000, 	"FireWire (IEEE 1394) controller (Generic)"						},
	{0x0C0010, 	"FireWire (IEEE 1394) controller (OHCI)"						},
  	{0x0C0100, 	"ACCESS Bus"										},
  	{0x0C0200, 	"SSA"											},
  	{0x0C0300, 	"USB (UHCI Controller)"									},
	{0x0C0310, 	"USB1.1 (OHCI Controller)"								},
	{0x0C0320, 	"USB2.0 (EHCI Controller)"								},
	{0x0C0330, 	"USB3.0 (XHCI Controller)"								},
	{0x0C0380, 	"USB Controller (Unspecified )"								},
	{0x0C03FE, 	"USB Device (Not a host controller)"							},
	{0x0C0400, 	"Fiber Channel"										},
  	{0x0C0500, 	"SMBus"											},
  	{0x0C0600, 	"InfiniBand"										},
	{0x0C0700, 	"IPMI Interface (SMIC)"									},
	{0x0C0701, 	"IPMI Interface (Keyboard Controller Style)"						},
	{0x0C0702, 	"IPMI Interface (Block Transfer)"							},
	{0x0C0800, 	"SERCOS Interface (IEC 61491"								},
	{0x0C0900, 	"CANbus"										},
  	{0x0D0000, 	"iRDA controller"									},
  	{0x0D0100, 	"Consumer IR controller"								},
  	{0x0D1000, 	"RF controller"										},
	{0x0D1100, 	"Bluetooth Controller"									},
	{0x0D1200, 	"Broadband Controller"									},
	{0x0D2000, 	"Ethernet Controller (802.1a – 5 GHz)"							},
	{0x0D2100, 	"Ethernet Controller (802.1b 2.4 GHz)"							},
  	{0x0D8000, 	"Other type of wireless controller"							},
  	{0x0E0000, 	"I2O"											},
  	{0x0F0000, 	"Satellite TV controller"								},
  	{0x0F0100, 	"Satellite audio controller"								},
  	{0x0F0300, 	"Satellite voice controller"								},
  	{0x0F0400, 	"Satellite data  controller"								},
  	{0x100000, 	"Network and Computing Encrpytion/Decryption"						},
  	{0x101000, 	"Entertainment Encryption/Decryption"							},
  	{0x108000, 	"Other Encryption/Decryption"								},
  	{0x110000, 	"DPIO module"										},
  	{0x110100, 	"Performance counters"									},
  	{0x111000, 	"Communication synchronizer"								},
  	{0x112000, 	"Signal Processing Management"								},
	{0x118000, 	"Other Signal Processing Controller"							},
  	{0x000000, 	"Null"											}
};

int read_pci_config_addr(int bus,int dev,int fun, int offset)
{
    	outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, offset));
    	return inportl(PCI_PORT_DATA);
}

void write_pci_config_addr(int bus,int dev,int fun,int offset,unsigned int data)
{
    	outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, offset));
    	outportl(PCI_PORT_DATA,data);
}

unsigned int pci_read_config_dword(int bus,int dev,int fun,int offset)
{
    	outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, offset));
    	return inportl(PCI_PORT_DATA);
}

void pci_write_config_dword(int bus,int dev,int fun,int offset,unsigned int data)
{
    	outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, offset));
    	outportl(PCI_PORT_DATA,data);
}

//FIXME Nelson, esta funcao deve retornar uma lista de trivers PCI presentes na maquina
int pci_get_info(void *buffer,int max_bus) 
{
	unsigned int data = -1;
	int bus, dev, fun,i;


	printf("PCI Listing devices:\n");

	for(bus = 0;bus < max_bus; bus++) {
		for(dev = 0; dev < MAX_DEV; dev++) {
			for(fun = 0; fun < MAX_FUN; fun++) {
				data = pci_read_config_dword(bus,dev,fun,8);
				if(data == -1)
					continue;
				for(i = 0;i < 256;i++) {
					if((pci_class_names[i].classcode &0xffffff) != (data >> 8 &0xffffff))
						continue;
					printf("%s",pci_class_names[i].name);
					printf(", B%d:D%d:F%d\n",bus,dev,fun); //FIXME
					break;

				}		//FIXME
				if(i == 256)printf("Other PCI Device ClassCode (%X), B%d:D%d:F%d\n",data,bus,dev,fun);
					
			}
		}
	}


	return (0);
}
// Esta funçao deve retornar o numero de barramento, o dispositivo e a funçao
// do dispositivo conectado ao barramento PCI, de acordo a classe.
unsigned int pci_scan_bcc(unsigned int bcc)
{
    unsigned int data = -1;

    int bus, dev, fun;

    
    for(bus = 0;bus < MAX_BUS; bus++) {
        for(dev = 0; dev < MAX_DEV; dev++){
            for(fun = 0; fun < MAX_FUN; fun++){

                outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, 0x8));
                data =inportl(PCI_PORT_DATA);

                if((data >> 24 &0xff) == bcc){

                    return (fun + (dev * 8) + (bus * 32));
            
                }
            }
     
        }
     
    }

    return (-1);

}

unsigned int pci_scan_class(int class)
{
    	unsigned int data = -1;

    	unsigned int bus, dev, fun;

	unsigned int r = 0;
    
    	for(bus = 0;bus < MAX_BUS; bus++) {
        	for(dev = 0; dev < MAX_DEV; dev++){
            		for(fun = 0; fun < MAX_FUN; fun++){

                		outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, 0x8));
                		data =inportl(PCI_PORT_DATA);

                		if((data >> 24 &0xff) == class){

					r = fun | dev << 16 | bus << 24;
                    			return r;
            
                		}
            		}
     
        	}
     
    	}

    return (-1);

}


unsigned int pci_scan_bcc_scc(unsigned int bcc,unsigned int scc)
{
    unsigned int data = -1;

    int bus, dev, fun;
    unsigned int r;

    
    for(bus = 0;bus < MAX_BUS; bus++){
        for(dev = 0; dev < MAX_DEV; dev++){
            for(fun = 0; fun < MAX_FUN; fun++){
                outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, 0x8));
                data =inportl(PCI_PORT_DATA);
                if(((data >> 24 &0xff) == bcc) && ((data >> 16 &0xff) == scc)){
                     
                    r = fun | dev << 16 | bus << 24;
                    return r;
            
                }
            }
     
        }
     
    }

    return (-1);

}

unsigned int pci_scan_bcc_scc_prog(unsigned int bcc,unsigned int scc,unsigned int prog)
{
    unsigned int data = -1;

    int bus, dev, fun;
    unsigned int r;

    
    for(bus = 0;bus < MAX_BUS; bus++){
        for(dev = 0; dev < MAX_DEV; dev++){
            for(fun = 0; fun < MAX_FUN; fun++){
                outportl(PCI_PORT_ADDR,CONFIG_ADDR(bus,dev,fun, 0x8));
                data =inportl(PCI_PORT_DATA);
                if(((data >> 24 &0xff) == bcc) && ((data >> 16 &0xff) == scc) && ((data >> 8 &0xff) == prog)){
                    r = fun | dev << 16 | bus << 24;
                    return r;
            
                }
            }
     
        }
     
    }

    return (-1);

}

unsigned int pci_scan_vendor(unsigned short vendor)
{


    return 0;
}

unsigned int pci_check_vendor(int bus,int dev,int fun,unsigned short vendor)
{



    return 0;
}

int pci_size(int base, int mask) 
{
  // Find the significant bits
  int size = mask & base;

  // Get the lowest of them to find the decode size
  size = size & ~(size-1);

  return size;
}
