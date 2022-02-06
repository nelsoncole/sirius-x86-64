#include <pci.h>
#include <idt.h>
#include <apic.h>
#include <stdio.h>

int vector_msi;
extern unsigned int localId;

extern void msi0();	extern void msi10();	extern void msi20();    extern void msi30();
extern void msi1();	extern void msi11();	extern void msi21();    extern void msi31();
extern void msi2();	extern void msi12();	extern void msi22();
extern void msi3();	extern void msi13();	extern void msi23();
extern void msi4();	extern void msi14();	extern void msi24();
extern void msi5();	extern void msi15();	extern void msi25();
extern void msi6();	extern void msi16();	extern void msi26();
extern void msi7();	extern void msi17();	extern void msi27();
extern void msi8();	extern void msi18();	extern void msi28();
extern void msi9();	extern void msi19();	extern void msi29();

void msi_vector_install()
{
    vector_msi = 0;

    idt_gate(0x80 + 0,(unsigned long)msi0 , 8,0);
    idt_gate(0x80 + 1,(unsigned long)msi1 , 8,0);
    idt_gate(0x80 + 2,(unsigned long)msi2 , 8,0);
    idt_gate(0x80 + 3,(unsigned long)msi3 , 8,0);
    idt_gate(0x80 + 4,(unsigned long)msi4 , 8,0);
    idt_gate(0x80 + 5,(unsigned long)msi5 , 8,0);
    idt_gate(0x80 + 6,(unsigned long)msi6 , 8,0);
    idt_gate(0x80 + 7,(unsigned long)msi7 , 8,0);
    idt_gate(0x80 + 8,(unsigned long)msi8 , 8,0);
    idt_gate(0x80 + 9,(unsigned long)msi9 , 8,0);

    idt_gate(0x80 +10,(unsigned long)msi10, 8,0);
    idt_gate(0x80 +11,(unsigned long)msi11, 8,0);
    idt_gate(0x80 +12,(unsigned long)msi12, 8,0);
    idt_gate(0x80 +13,(unsigned long)msi13, 8,0);
    idt_gate(0x80 +14,(unsigned long)msi14, 8,0);
    idt_gate(0x80 +15,(unsigned long)msi15, 8,0);
    idt_gate(0x80 +16,(unsigned long)msi16, 8,0);
    idt_gate(0x80 +17,(unsigned long)msi17, 8,0);
    idt_gate(0x80 +18,(unsigned long)msi18, 8,0);
    idt_gate(0x80 +19,(unsigned long)msi19, 8,0);

    idt_gate(0x80 +20,(unsigned long)msi20, 8,0);
    idt_gate(0x80 +21,(unsigned long)msi21, 8,0);
    idt_gate(0x80 +22,(unsigned long)msi22, 8,0);
    idt_gate(0x80 +23,(unsigned long)msi23, 8,0);
    idt_gate(0x80 +24,(unsigned long)msi24, 8,0);
    idt_gate(0x80 +25,(unsigned long)msi25, 8,0);
    idt_gate(0x80 +26,(unsigned long)msi26, 8,0);
    idt_gate(0x80 +27,(unsigned long)msi27, 8,0);
    idt_gate(0x80 +28,(unsigned long)msi28, 8,0);
    idt_gate(0x80 +29,(unsigned long)msi29, 8,0);

    idt_gate(0x80 +30,(unsigned long)msi30, 8,0);
    idt_gate(0x80 +31,(unsigned long)msi31, 8,0);
}


void default_mas(int n){
	
	printf("Null MSI vector: %d\n",n+0x80); 
}

static void *fnvetors_handler_msi[32] = {
    &default_mas, // 0
    &default_mas, // 1
    &default_mas, // 2
    &default_mas, // 3
    &default_mas, // 4
    &default_mas, // 5
    &default_mas, // 6
    &default_mas, // 7
    &default_mas, // 8
    &default_mas, // 9
    &default_mas, // 10
    &default_mas, // 11
    &default_mas, // 12
    &default_mas, // 13
    &default_mas, // 14
    &default_mas, // 15
    &default_mas, // 16
    &default_mas, // 17
    &default_mas, // 18
    &default_mas, // 19
    &default_mas, // 20
    &default_mas, // 21
    &default_mas, // 22
    &default_mas, // 23
    &default_mas, // 24
    &default_mas, // 25
    &default_mas, // 26
    &default_mas, // 27
    &default_mas, // 28
    &default_mas, // 29
    &default_mas, // 30
    &default_mas, // 31
};

extern void call_function(void *, int);
void msi_function(unsigned int n){

	//n -= 0x80;

    if(n < 32){

    	void *addr = fnvetors_handler_msi[n];
    	// Chamda de função correspondente
    	call_function(addr, n);

    }else printf("error msi vector: %d\n", n+0x80);

    apic_eoi_register();
	
}

int apic_send_msi( struct dev *dev, void (*fuc)()){

    int bus = dev->bus;
    int slot = dev->slot;
    int function = dev->function;

    unsigned int command = 0;

    unsigned short capp_addr = 0;

    // MSI
    // Capabilities List Pointer
    capp_addr = pci_read_config_byte(bus,slot,function, 0x34);
    for(int i=0; i < 10; i++){
        // Capabilities List Pointer Register
        command = pci_read_config_word(bus,slot,function, capp_addr);
        if((command&0xff) == 0x5) break;

        capp_addr = command >> 8 &0xff;
        if(!capp_addr) return -1;

    }

    command = pci_read_config_word(bus,slot,function, capp_addr);
    if((command&0xff) !=0x5)
        return -1;

    // INTx# Desable
    command = pci_read_config_word(bus,slot,function, 0x4); 
    command |= 1 << 10;
    pci_write_config_word(bus,slot,function, 0x4, command);

    unsigned long long MSI_TAG = 0xFEE00000 | (localId << 12);
    pci_write_config_dword(bus,slot,function, capp_addr + 0x4, MSI_TAG);
    pci_write_config_dword(bus,slot,function, capp_addr + 0x8, MSI_TAG >> 32);

    // (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
    command = (0x80 + vector_msi) & 0xFF; 
    pci_write_config_word(bus,slot,function, capp_addr + 0xc, command);

    // MSI Enable
    command = pci_read_config_word(bus,slot,function, capp_addr + 0x2);
    command |= 0x1;
    pci_write_config_word(bus,slot,function, capp_addr + 0x2, command);

    fnvetors_handler_msi[vector_msi++] = (void*)fuc;
    return 0;

}

int apic_send_msix( struct dev *dev, void (*fuc)()){
    /*

    // MSI-X
    // Capabilities List Pointer
    capp = pci_read_config_byte(bus,slot,function, 0x34);
    for(int i=0; i < 10; i++){
        // Capabilities List Pointer Register
        command = pci_read_config_word(bus,slot,function, capp);
        if((command&0xff) == 0x11) break;

        capp = command >> 8 &0xff;
        if(!capp) break;

    }

    if(capp != 0){
        command = pci_read_config_word(bus,slot,function, capp);
        if((command&0xff) ==0x11){

            // INTx# Disable
            command = pci_read_config_word(bus,slot,function, 0x4) | 1 << 10;
            //pci_write_config_word(bus,slot,function, 0x4, command);

            int msix_off = capp;
            unsigned int msix_bir = pci_read_config_dword(bus,slot,function,capp + 4);
            unsigned int msix_table_off = msix_bir & ~7;
            unsigned long tmp;

            msix_bir &= 7;

            int msix_sz = pci_read_config_word(bus,slot,function, capp + 0x2) &0x7FF;


            tmp = pci_read_config_dword(bus,slot,function, 0x10 + msix_bir*4 + 4);
            tmp <<= 32;
            tmp |= pci_read_config_dword(bus,slot,function, 0x10 + msix_bir*4) &0xfffffff0;
            tmp += msix_table_off;

            mm_mp(tmp, (unsigned long*)&tmp, 0x40000, 0);
            unsigned int *msix = (unsigned int *)tmp;
            //char *msix_bitmap = malloc(msix_sz);

            unsigned int pba_bir = pci_read_config_dword(bus,slot,function,capp + 8);
            unsigned int pba_table_off = msix_bir & ~7;
            pba_bir &= 7;

            tmp = pci_read_config_dword(bus,slot,function, 0x10 + pba_bir*4 + 4);
            tmp <<= 32;
            tmp |= pci_read_config_dword(bus,slot,function, 0x10 + pba_bir*4) &0xfffffff0;
            tmp += pba_table_off;
            unsigned long *pba = (unsigned long *)tmp;
        
            // Mask everything
            unsigned int ctrl = pci_read_config_dword(bus,slot,function, msix_off);
            pci_write_config_dword(bus,slot,function, msix_off, ctrl | (1 << 30));

            for (int i = 0; i < msix_sz; i++) {
                msix[i * 4 + 0] = 0;
                msix[i * 4 + 1] = 0;
                msix[i * 4 + 2] = 0;
                msix[i * 4 + 3] = 1;
            }


            int i = 0;
            int irq = 0;
            unsigned long MSI_TAG = 0xFEE00000;
            msix[i * 4 + 0] = MSI_TAG | (localId << 12);
            msix[i * 4 + 1] = MSI_TAG >> 32;
            msix[i * 4 + 2] = irq & 0xFF;
            msix[i * 4 + 3] = 0;
            // unmask everything
            pci_write_config_dword(bus,slot,function, msix_off, ctrl & ~(1 << 30));

            // MSI-x Enable
            command = pci_read_config_dword(bus,slot,function, capp);
            command |= 0x80000000;
            pci_write_config_dword(bus,slot,function, capp, command);

        }
    }

    */
    return -1;
}
