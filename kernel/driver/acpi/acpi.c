#include <acpi.h>
#include <io.h>
#include <mm.h>

#include <sleep.h>

#include <stdio.h>
#include <string.h>


ACPI_TABLE_RSDP *rsdp;
ACPI_TABLE_RSDT *rsdt;
ACPI_TABLE_XSDT *xsdt;
ACPI_TABLE_FADT *fadt;
ACPI_TABLE_DSDT *dsdt;
ACPI_TABLE_HPET *hpet;


unsigned short SLP_TYPa;
unsigned short SLP_TYPb;


unsigned long acpi_virtual_addr;
unsigned long acpi_physical_init;

static unsigned long acpi_set_virtual_addr(unsigned long phy_addr) {
	
	return (acpi_virtual_addr + (phy_addr - acpi_physical_init) );
} 

static int init_rsdp(unsigned long ptr)
{	

	rsdp = (ACPI_TABLE_RSDP*) ptr;
	
	char checksum = 0;
	
	// check checksum RSDP
      	char *bptr = (char *) ptr;
      
      	for (int i=0; i < 20; i++) {
         	checksum += *bptr;
         	bptr++;
      	}
      	
      	if(checksum){
      		printf("Checksum failed\n");
      		return 1;
      	}
	
	char signature[9];
	char oem[7];
	memcpy(signature,rsdp->signature,8);
	memcpy(oem,rsdp->oem_id,6);
	signature[8] = 0;
	oem[6] = 0;
	
	// TODO: Aqui mapeamos possiveis endereços ACPI
	acpi_physical_init = rsdp->rsdt_addr &0xFFF00000;
	
	mm_mp(acpi_physical_init, &acpi_virtual_addr, 0x400000 /*4MiB*/, 0);
	
	if(rsdp->revision == 0x2){
	
		printf("ACPI version 2.0, ");
		
	
		rsdt = (ACPI_TABLE_RSDT*) acpi_set_virtual_addr(rsdp->rsdt_addr);
		xsdt = (ACPI_TABLE_XSDT*) acpi_set_virtual_addr(rsdp->xsdt_addr);
		
	
	}else if(rsdp->revision == 0) {
	
		printf("ACPI version 1.0, ");
		
		rsdt = (ACPI_TABLE_RSDT*) acpi_set_virtual_addr(rsdp->rsdt_addr);
		
		xsdt = (ACPI_TABLE_XSDT*) 0;
		
	
	}
	
	printf("OEM_ID \"%s\"\n",oem);
	
	return 0;
}


static int acpi_check_header(unsigned int *ptr, char *sig)
{
	unsigned int sig1 = *(unsigned int*)ptr;
	unsigned int sig2 = *(unsigned int*)sig;

   	if ( sig1 == sig2) 
   		return 0;
   		
   	return -1;
}

unsigned long acpi_probe(ACPI_TABLE_RSDT *_rsdt, ACPI_TABLE_XSDT *_xsdt, char *signature)
{

	unsigned long ptr = (unsigned long)&rsdt->entry;
	unsigned int *array = (unsigned int*) ptr;
	
	int len =  rsdt->length - 36;
	
     	for(int i = 0; i < len; i +=4) {
	
		ptr = acpi_set_virtual_addr(*array++);
        		
        	if(acpi_check_header((unsigned int*) ptr, signature) == 0)
        		return ptr;
        }

	return 0;
}


int acpi_enable(void)
{
	// verifica se acpi ja esta habilitada
	if((inportw(fadt->PM1a_CNT_BLK) &1) == 0){
	
		//printf("acpi esta desabilitada\n");
		
		// verificar se acpi pode ser habilitado
		if ( fadt->SMI_CMD != 0 && fadt->ACPI_ENABLE != 0) {
		
			//printf("Habilitar o acpi ...\n");
			
			// enviar comando acpi enable
			outportb(fadt->SMI_CMD, fadt->ACPI_ENABLE);
			 
        		// give 3 seconds time to enable acpi
        		// Linux epera 3 segundos 
        		int i;
         		for (i=0; i<10; i++ )
         		{
            			if ( (inportw(fadt->PM1a_CNT_BLK) &1) == 1 ){
            				//printf("acpi esta agora habilitada.\n");
               			break;
               		}
               		
            			wait(30000000);
         		}
         		
         		if (fadt->PM1b_CNT_BLK != 0) {
            			for (; i<10; i++ )
            			{
               			if ( (inportw(fadt->PM1b_CNT_BLK) &1) == 1 ) {
               			
                  				break;
                  			}
               			
               			wait(30000000);
            			}
            		}
            		
            		// verificar se acpi foi habilitado
         		if (i<10) {
            			//printf("acpi habilitado\n");
            			return 0;
         		} else {
            			//printf("nao foi possível habilitar acpi\n");
            			return -1;
         		}
		
		
		} else {
			
			//printf("acpi nao pode ser habilitado");
			return -1;
		}
	
	}else {
	
		//printf("acpi ja esta habilitada\n");
	}

	return 0;
}

static unsigned long acip_init() {

	// // search below the 1mb mark for RSDP signature
	unsigned long ptr = 0x000E0000;
	unsigned long end = ptr + 0x100000;
	
	unsigned int ebda = (unsigned int) (*(unsigned short *) EBDA);
	ebda = ebda << 4;  // convert segment em linear address
	

    	while ( ptr  < end ) {
    	
        	unsigned long long signature = *(unsigned long long *) ptr;

        	if (signature == 0x2052545020445352) // "RSD PTR "
      		{
            		return ptr;
        	}
        	
        	ptr  += 16;
    	}
    	
    	
    	// search Extended BIOS Data Area for the Root System Description Pointer signature
   	ptr = ebda;
   	end = ptr + 1024;
   	
   	while(ptr  < end){
   	
   		unsigned long long signature = *(unsigned long long *) ptr;

        	if (signature == 0x2052545020445352) // "RSD PTR "
      		{
            		return ptr;
        	}
        	
        	ptr  += 16;
   	
   	}
   	

	return 0;
}


void setup_acpi()
{

	printf("ACPI_Setup ... \\\\ \n");

	unsigned long ptr = acip_init();
   	if(ptr){
   	
   		// call RSDP
        	init_rsdp( ptr);
        	
        	// call FADT
        	fadt = (ACPI_TABLE_FADT*) acpi_probe(rsdt, xsdt, "FACP");
      
        	
    	}else {
    	
    		printf("acpi error\n");
    		return;
    	}   	
   	
   	ptr = acpi_set_virtual_addr(fadt->DSDT);
 	dsdt = (ACPI_TABLE_DSDT*) ptr;
 	
 	if(acpi_check_header((unsigned int*) ptr, "DSDT") == 0)
        {	
        	//search the \_S5 package in the DSDT
        	char *aml_code= (char *) &dsdt->definition_block;
              	int len = dsdt->length;
          
              	while(len--) { 
              	
              		
              		if(memcmp(aml_code, "_S5_", 4) == 0){
              		
              			aml_code += 5;
                     		aml_code += ((*aml_code &0xC0)>>6) +2;   // calcular PkgLength size
              			
              			if (*aml_code == 0x0A)
                        		aml_code++;   // skip byteprefix
                        		
                        	SLP_TYPa = *(aml_code)<<10;
                     		aml_code++;
                     		
                     		if (*aml_code == 0x0A)
                        		aml_code++;   // skip byteprefix
                     		SLP_TYPb = *(aml_code)<<10;
                     		
                        
              			break;
              		}
              		
              		aml_code++;
              	}
        	
        
        }else {
        
        	printf("DSDT nao encontrado\n");
        }
 	   
 	acpi_enable();
 	
}

// Credito: kaworu (https://forum.osdev.org/viewtopic.php?t=16990)
void poweroff() {

   	// enviar o comando shutdown
   	outportw(fadt->PM1a_CNT_BLK, SLP_TYPa | 1<<13 );
   	
   	if ( fadt->PM1b_CNT_BLK != 0 ) {
 
      		outportw(fadt->PM1b_CNT_BLK, SLP_TYPb | 1<<13 );
      		
	}
	
}


void reboot()
{
	cli();
	
	if(rsdp->revision >= 0x2) {
		outportb(fadt->ResetRegister.address, fadt->ResetValue);
	}	

	printf("Use the 8042 keyboard controller to pulse the CPU's RESET pin\n");
	
	unsigned char val = 0x02;
    	while ( val & 0x02) {
    		
      		val = inportb(0x64);
    	}		
    		
    	outportb(0x64, 0xFE);
    		
    	while(1) hlt();
	
}
