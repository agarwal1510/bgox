#include<sys/defs.h>
#include<sys/kprintf.h>
#include <sys/portio.h>
#include <sys/ahci.h>
#define BUS_NUM 256
#define DEVICE_NUM 32
#define FUNC_NUM 8
#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA	0xCFC

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier


static int check_type(hba_port_t *port) {
         //uint32_t ssts = port->ssts;
         //uint8_t ipm = (ssts >> 8) & 0x0F;
         //uint8_t det = ssts & 0x0F;

         switch (port->sig) {
                 case SATA_SIG_ATAPI:
                         return AHCI_DEV_SATAPI;
                 case SATA_SIG_SEMB:
                         return AHCI_DEV_SEMB;
                 case SATA_SIG_PM:
                         return AHCI_DEV_PM;
                 default:
                         return AHCI_DEV_SATA;
	}
}


void probe_port(hba_mem_t *abar) {
	uint32_t pi = abar->pi;
	int i = 0;
	while (i < 32) {
		if (pi & 1) {
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
				kprintf("SATA drive found at port %d\n", i);
			else if (dt == AHCI_DEV_SATAPI)
				kprintf("SATAPI drive found at port %d\n", i);
			else if (dt == AHCI_DEV_SEMB)
				kprintf("SEMB drive found at port %d\n", i);
			else if (dt == AHCI_DEV_PM)
				kprintf("PM drive found at port %d\n", i);
			else
				kprintf("No drive found at port %d\n", i);

		}
		pi >>= 1;
		i++;
	}
}

uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
		uint32_t address;
		uint32_t lbus  = (uint32_t)bus;
		uint32_t lslot = (uint32_t)slot;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;

		/* create configuration address as per Figure 1 */
		address = (uint32_t)((lbus << 16) | (lslot << 11) |
						(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

		/* write out the address */
		outl(0xCF8, address);
		/* read in the data */
		/* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
		tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
		//tmp = (uint16_t)inl(0xCFC);
		return (tmp);
}

void check_device(uint8_t bus_num, uint8_t device_num){
		uint8_t func = 0;
		uint16_t deviceid;
		uint16_t vendorid;
		//uint8_t classid;
		uint16_t subclassid;
		
	vendorid = pciConfigReadWord(bus_num, device_num, func, 0);
	if (vendorid == 0xFFFF)
		return;
	else {
		deviceid = pciConfigReadWord(bus_num, device_num, func, 2);
		subclassid = pciConfigReadWord(bus_num, device_num, func, 0x0A);
		//classid = pciConfigReadWord(bus_num, device_num, func, 0x0B);
		if (subclassid == 0x0106)
			kprintf("%x %x %x ", vendorid, deviceid, subclassid);
		
	}
}

void enumerate_pci(){
	for(uint8_t i = 0;i < 256; i++){
		for(uint8_t j = 0; j < DEVICE_NUM; j++){
			check_device(i, j);
		}
	
	}

}

void find_ahci(){
	enumerate_pci();
}
