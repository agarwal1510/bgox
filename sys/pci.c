#include<sys/defs.h>
#include<sys/kprintf.h>
#include <sys/portio.h>
#define BUS_NUM 256
#define DEVICE_NUM 32
#define FUNC_NUM 8
#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA	0xCFC

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
