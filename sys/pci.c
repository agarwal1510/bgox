#include<sys/defs.h>
#include<sys/kprintf.h>
#include <sys/portio.h>
#include <sys/ahci.h>
#define BUS_NUM 256
#define DEVICE_NUM 32
#define FUNC_NUM 8
#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA	0xCFC
uint64_t ahci_addr;
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1 
#define AHCI_DEV_NULL 0 

// Check device type
int count = 0;
static int check_type(hba_port_t *port)
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
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

void probe_port(hba_mem_t *abar)
{
	// Search disk in impelemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i<32)
	{	
		//kprintf("%d", pi);	
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}
}
 
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
		uint32_t address;
		uint32_t lbus  = (uint32_t)bus;
		uint32_t lslot = (uint32_t)slot;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;

		address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

		outl(0xCF8, address);
		outl(0xCFC, 0x298B6040);
		tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
		return (tmp);
}

int check_device(uint8_t bus_num, uint8_t device_num){
		uint8_t func = 0;
		uint16_t vendorid;
		uint16_t class_scId;
		vendorid = pciConfigReadWord(bus_num, device_num, func, 0);

		if (vendorid == 0xFFFF)
				return -1;

		class_scId = pciConfigReadWord(bus_num, device_num, func, 0x0A);
		if (class_scId == 0x0106){
				uint16_t l_addr = pciConfigReadWord(bus_num, device_num, func, 0x24);
				uint16_t h_addr = pciConfigReadWord(bus_num, device_num, func, 0x26);
				ahci_addr = (uint32_t)h_addr << 16 | l_addr;
				kprintf("AHCI Found at address: %x\n", ahci_addr);
				return 1;
		}
		return -1;
}

void enumerate_pci(){
		uint8_t i, j;
		int found = 0;
		for(i = 0; found == 0 && i < BUS_NUM; i++){
				for(j = 0; j < DEVICE_NUM; j++){
						if (check_device(i, j) > 0){
								found = 1;
								break;
						}
				}
		}
		if (found == 1){
				i--;
				kprintf("AHCI Detected Bus: %d Device: %d\n", i, j);
		}
		//kprintf("Count: %d", count);
}

void find_ahci(){
		enumerate_pci();
		probe_port((hba_mem_t *)(ahci_addr));
}
