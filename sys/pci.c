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
#define	AHCI_BASE	0x400000	// 4M
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define DWORD uint32_t
#define WORD uint16_t
#define BYTE uint8_t
#define FALSE 0
#define TRUE 1

void memset(void *address, int value, int size) {
        unsigned char *p = address;
        for (int i = 0; i< size; i++)
                *p++ = (unsigned char)value;
}
 
// Find a free command list slot
int find_cmdslot(hba_port_t *m_port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (m_port->sact | m_port->ci);
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}


int read(hba_port_t *port, uint32_t startl, uint32_t  starth, uint32_t count, uint16_t *buf)
{
	port->is_rwc = (DWORD)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int i = 0;
	int slot = find_cmdslot(port);
	if (slot == -1)
		return FALSE;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t *)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t *)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth>>8);
 
	cmdfis->count = count;
	//cmdfis->counth = HIBYTE(count);
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return FALSE;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return FALSE;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return FALSE;
	}
 
	return TRUE;
}

// Start command engine
void start_cmd(hba_port_t *port)
{
        // Wait until CR (bit15) is cleared
        while (port->cmd & HBA_PxCMD_CR);

        // Set FRE (bit4) and ST (bit0)
        port->cmd |= HBA_PxCMD_FRE;
        port->cmd |= HBA_PxCMD_ST;
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
        // Clear ST (bit0)
        port->cmd &= ~HBA_PxCMD_ST;

        // Wait until FR (bit14), CR (bit15) are cleared
        while(1)
        {
                if (port->cmd & HBA_PxCMD_FR)
                        continue;
                if (port->cmd & HBA_PxCMD_CR)
                        continue;
                break;
        }
 
        // Clear FRE (bit4)
        port->cmd &= ~HBA_PxCMD_FRE;
}

void port_rebase(hba_port_t *port, int portno)
{
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	//port->clbu = 0;
	
	memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	//port->fbu = 0;
	
	memset((void*)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		//cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
}
 
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
				//port_rebase(&abar->ports[i], i);
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
