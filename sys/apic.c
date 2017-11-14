#define MAX_IDT 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define KB_DATA	0x60
#define KB_STATUS 0x64
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/portio.h>
#include <sys/apic.h>
#include <sys/mem.h>
//#include <sys/apicio.h>
#include <sys/ptmgr.h>
#include "kb_map.h"
#define CPUID_FLAG_APIC 1 << 9
#define CPUID_FLAG_MSR 1 << 5
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800
uint32_t *APIC_BASE = (uint32_t *)0xFE00000;
uint32_t *IOAPIC_BASE = (uint32_t *)0xFC00000;
extern void load_idt(unsigned long *idt_ptr);
extern void isr0(void);
//extern void isr1(void);
extern void timer_init(void);
extern unsigned char kbdus[128];
int SHIFT_ON = 0;
int CTRL_ON = 0;

static AcpiMadt *s_madt;

struct gate_str{
		uint16_t offset_low;     // offset bits 0...15
		uint16_t selector;       // a code segment selector in GDT or LDT 16...31
		uint8_t zero;           // unused set to 0, set at 96...127
		uint8_t type_attr;       // type and attributes 40...43, Gate Type 0...3
		uint16_t offset_mid;  // offset bits 16..31, set at 48...63
		uint32_t offset_high;  // offset bits 16..31, set at 48...63
		uint32_t ist;           // unused set to 0, set at 96...127
}  __attribute__((packed));

struct gate_str IDT[MAX_IDT];

int ticks = 0;
int sec = -1;
struct idtr_t
{
		uint16_t size;
		uint64_t addr;
} __attribute__((packed));

struct idtr_t idtr = {((sizeof(struct gate_str))*MAX_IDT), (uint64_t)&IDT};

void _x86_64_asm_lidt(struct idtr_t *idtr);

static inline void apicwrite(void *reg, uint32_t value)
{
//		kprintf("\nWriting: %x %x\n", reg, value);
//		uint64_t volatile *localapic = (uint64_t volatile *)reg;
//		*localapic = value;
		uint32_t volatile *localapic = (uint32_t volatile *)(reg);
		*(localapic) = value;
}
static inline uint32_t apicread(void *reg)
{
//		kprintf("reading: %x\n", reg);
	//	uint64_t volatile *localapic = (uint64_t volatile *)(reg);
		uint32_t volatile *localapic = (uint32_t volatile *)(reg);
		return *(localapic);
}

void irq_timer_handler(void){
//	apicwrite(IOAPIC_BASE + LAPIC_EOI, 0);
	outb(0x20, 0x20);
	outb(0xa0, 0x20);
	kprintf("Entere\n");
//	if (ticks % 18 == 0){
//		int new_sec = ticks/100;
//		if (new_sec != sec){
//			sec = new_sec;
//			kprintf_boott(">Time Since Boot: ", sec);
//		}
//	}
//	ticks++;
}
void memcpy(void *dest, void* src, int size ){
	char *source = (char *)src;
	char * dst = (char *)dest;
	for(int i = 0; i < size; i ++){
		dst[i] = source[i];
	}
}
/*
void irq_kb_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);

	char status = inb(KB_STATUS);
	if (status & 0x01){
//		outb(KB_STATUS, 0x20);
		unsigned int key = inb(KB_DATA);
		if (key < 0){
			kprintf("OOPS");
			return;
		}
		if (key == 0x2a){
			SHIFT_ON = 1;
			return;
		}
		else if (key == 0xaa){
			SHIFT_ON = 0;
			return;
		}
		if (key == 0x1d){
			CTRL_ON = 1;
			SHIFT_ON = 1; // should be ^C and not ^c
			kprintf_at("%c", 146, 24, '^');
			return;
		}
		else if (key == 0x9d){
			CTRL_ON = 0;
			SHIFT_ON = 0;
			return;
		}
		if (key < 0x52){ // Keys over SSH
			if (CTRL_ON == 0)
				kprintf_at("%c", 146, 24, ' ');
			if (SHIFT_ON == 1){
				switch (key){
					case 2:
						kprintf_at("%c", 148, 24, '!');
						break;
					case 3:
						kprintf_at("%c", 148, 24, '@');
						break;
					case 4:
						kprintf_at("%c", 148, 24, '#');
						break;
					case 5:
						kprintf_at("%c", 148, 24, '$');
						break;
					case 6:
						kprintf_at("%c", 148, 24, '%');
						break;
					case 7:
						kprintf_at("%c", 148, 24, '^');
						break;
					case 8:
						kprintf_at("%c", 148, 24, '&');
						break;
					case 9:
						kprintf_at("%c", 148, 24, '*');
						break;
					case 10:
						kprintf_at("%c", 148, 24, '(');
						break;
					case 11:
						kprintf_at("%c", 148, 24, ')');
						break;
					case 12:
						kprintf_at("%c", 148, 24, '_');
						break;
					case 13:
						kprintf_at("%c", 148, 24, '+');
						break;
					case 26:
						kprintf_at("%c", 148, 24, '{');
						break;
					case 27:
						kprintf_at("%c", 148, 24, '}');
						break;
					case 39:
						kprintf_at("%c", 148, 24, ':');
						break;
					case 40:
						kprintf_at("%c", 148, 24, '"');
						break;
					case 41:
						kprintf_at("%c", 148, 24, '~');
						break;
					case 43:
						kprintf_at("%c", 148, 24, '|');
						break;
					case 51:
						kprintf_at("%c", 148, 24, '<');
						break;
						case 52:
						kprintf_at("%c", 148, 24, '>');
						break;
						case 53:
						kprintf_at("%c", 148, 24, '?');
						break;
						default:
						kprintf_at("%c", 148, 24, kbdus[key] - 32);	
						break;
						}
}
else
kprintf_at("%c", 148, 24, kbdus[key]);
}
}
}*/

void pic_init(void){ // SETUP Master and Slave PICS
		outb(0x20, 0x11);
		outb(0xA0, 0x11); 
		outb(0x21, 0x20);
		outb(0xA1, 0x28);
		outb(0x21, 0x04);
		outb(0xA1, 0x02);
		outb(0x21, 0x01);
		outb(0xA1, 0x01);
		//    outb(0x21, 0xff);
		//	outb(0xA1, 0xff);
}
void pic_disable()
{
		/* Set OCW1 (interrupt masks) */
		outb(0x21, 0xFF);
		outb(0xA1, 0xFF);
}
void apic_start_timer() {
		// Tell APIC timer to use divider 16
		apicwrite(APIC_BASE + LAPIC_TDCR, 0x3);

		// Prepare the PIT to sleep for 10ms (10000µs)
		//    pit_prepare_sleep(10000);

		// Set APIC init counter to -1
//		apicwrite(APIC_BASE + LAPIC_TICR, 0xffffffff);

		// Perform PIT-supported sleep
		//      pit_perform_sleep();

		// Stop the APIC timer
//		apicwrite(APIC_BASE + LAPIC_TIMER, 0x10000);
		//  apicwrite(APIC_REGISTER_LVT_TIMER, APIC_LVT_INT_MASKED);

		// Now we know how often the APIC timer has ticked in 10ms
		//uint32_t ticksIn10ms = 0xFFFFFFFF - apicread(APIC_REGISTER_TIMER_CURRCNT);

		// Start timer as periodic on IRQ 0, divider 16, with the number of ticks we counted
		//    apicwrite(APIC_REGISTER_LVT_TIMER, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
		//    apicwrite(APIC_REGISTER_TIMER_DIV, 0x3);
//		 uint32_t ticksIn10ms = 0x1;
		apicwrite(APIC_BASE + LAPIC_TIMER, 32 | 0x00000); //One - Shot
		apicwrite(APIC_BASE + LAPIC_TDCR, 0x3);
		apicwrite(APIC_BASE + LAPIC_TICR, 0x1);
		kprintf("timer: %x", apicread(APIC_BASE + LAPIC_TIMER));
}
void cpuid(uint32_t a, uint32_t *b){
		__asm__("cpuid"
						:"=a"(*b)                 // EAX into b (output)
						:"0"(a)                  // a into EAX (input)
						:"%ebx","%ecx","%edx");  // clobbered registers
}

int check_apic() {
		uint32_t edx;
		cpuid(1, &edx);
		return edx & CPUID_FLAG_APIC;
}
int check_msr() {
		uint32_t edx;
		cpuid(1, &edx);
		return edx & CPUID_FLAG_MSR;
}
void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
		__asm__ volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi)
{
		__asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void cpu_set_apic_base(uintptr_t apic) {
		uint32_t edx = 0;
		uint32_t eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;

#ifdef __PHYSICAL_MEMORY_EXTENSION__
		edx = (apic >> 32) & 0x0f;
#endif

		cpuSetMSR(IA32_APIC_BASE_MSR, eax, edx);
}

uintptr_t cpu_get_apic_base() {
		uint32_t eax, edx;
		cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);


#ifdef __PHYSICAL_MEMORY_EXTENSION__
		return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
#else
		return (eax & 0xfffff000);
#endif
}
void setup_gate(int32_t num, uint64_t handler_addr){
		IDT[num].offset_low = (handler_addr & 0xFFFF);
		IDT[num].offset_mid = ((handler_addr >> 16) & 0xFFFF);
		IDT[num].offset_high = ((handler_addr >> 32) & 0xFFFFFFFF);
		IDT[num].selector = 0x08;
		IDT[num].type_attr = 0x8e;
		IDT[num].zero = 0x0;
		IDT[num].ist = 0x0;
}
void idt_init(void)
{
//	for(int i = 0; i < 255; i ++)
//		setup_gate(i, (uint64_t)isr0);
		setup_gate(32, (uint64_t)isr0);
		setup_gate(0xff, (uint64_t)isr0);
//		setup_gate(31, (uint64_t)isr0);
//			setup_gate(33, (uint64_t)isr0);
		_x86_64_asm_lidt(&idtr);
}

   void mask_init(void){
   outb(0x21 , 0xFC); //11111100
   }
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// ------------------------------------------------------------------------------------------------
// IO APIC Registers
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

// ------------------------------------------------------------------------------------------------
static void IoApicOut(uint64_t *base, uint8_t reg, uint32_t val)
{
    apicwrite(base + IOREGSEL, reg);
    apicwrite(base + IOWIN, val);
}

// ------------------------------------------------------------------------------------------------
static uint32_t IoApicIn(uint64_t *base, uint8_t reg)
{
    apicwrite(base + IOREGSEL, reg);
    return apicread(base + IOWIN);
}

// ------------------------------------------------------------------------------------------------
void IoApicSetEntry(uint32_t *base, uint8_t index, uint64_t data)
{
    IoApicOut(base, IOREDTBL + index * 2, (uint32_t)data);
    kprintf("***Here: %x %x***", data, IoApicIn(base, IOREDTBL + index* 2));
	IoApicOut(base, IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}

// ------------------------------------------------------------------------------------------------
void IoApicInit()
{
    // Get number of entries supported by the IO APIC
    uint32_t x = IoApicIn(IOAPIC_BASE, IOAPICVER);
    kprintf("x: %p", x);
    int count = (((uint32_t)x >> (uint32_t)16) & 0xff) + 1;    // maximum redirection entry


    // Disable all entries
    for (int i = 0; i < count; ++i)
    {
//    	kprintf("Here at SetEntry\n");
        IoApicSetEntry(IOAPIC_BASE, i, 1 << 16);
    }
    kprintf("I/O APIC pins = %d\n", count);
}

static void AcpiParseApic(AcpiMadt *madt)
{
    s_madt = madt;

    kprintf("Local APIC Address = 0x%x\n", madt->localApicAddr);
    APIC_BASE = (uint64_t *)(uintptr_t)madt->localApicAddr;

    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    while (p < end)
    {
        ApicHeader *header = (ApicHeader *)p;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == APIC_TYPE_LOCAL_APIC)
        {
            ApicLocalApic *s = (ApicLocalApic *)p;

            kprintf("Found CPU: %d %d %x\n", s->acpiProcessorId, s->apicId, s->flags);
      //      if (g_acpiCpuCount < MAX_CPU_COUNT)
       //     {
        //        g_acpiCpuIds[g_acpiCpuCount] = s->apicId;
         //       ++g_acpiCpuCount;
          //  }
        }
        else if (type == APIC_TYPE_IO_APIC)
        {
            ApicIoApic *s = (ApicIoApic *)p;

            kprintf("Found I/O APIC: %d 0x%x %d\n", s->ioApicId, s->ioApicAddress, s->globalSystemInterruptBase);
            IOAPIC_BASE = (uint64_t *)(uintptr_t)s->ioApicAddress;
        }
        else if (type == APIC_TYPE_INTERRUPT_OVERRIDE)
        {
   //         ApicInterruptOverride *s = (ApicInterruptOverride *)p;

//            kprintf("Found Interrupt Override: %d %d %d 0x%04x\n", s->bus, s->source, s->interrupt, s->flags);
        }
        else
        {
  //          kprintf("Unknown APIC structure %d\n", type);
        }

        p += length;
    }
}
static void AcpiParseDT(AcpiHeader *header)
{
    uint32_t signature = header->signature;

    char sigStr[5];
    memcpy(sigStr, &signature, 4);
    sigStr[4] = 0;
    kprintf("%s 0x%x\n", sigStr, signature);

    if (signature == 0x50434146)
    {
		kprintf("sig: 1\n");
  //      AcpiParseFacp((AcpiFadt *)header);
    }
    else if (signature == 0x43495041)
    {
//		kprintf("sig2: \n");
        AcpiParseApic((AcpiMadt *)header);
    }
}

static void AcpiParseRsdt(AcpiHeader *rsdt)
{
    uint32_t *p = (uint32_t *)(rsdt + 1);
    uint32_t *end = (uint32_t *)((uint8_t*)rsdt + rsdt->length);

    while (p < end)
    {
        uint32_t address = *p++;
        AcpiParseDT((AcpiHeader *)(uintptr_t)address);
    }
}
static int AcpiParseRsdp(uint8_t *p)
{
    // Parse Root System Description Pointer
//    ConsolePrint("RSDP found\n");

    // Verify checksum
    uint8_t sum = 0;
    for (int i = 0; i < 20; ++i)
    {
        sum += p[i];
    }

    if (sum)
    {
  //      ConsolePrint("Checksum failed\n");
        return 0;
    }

    // Print OEM
    char oem[7];
    memcpy(oem, p + 9, 6);
    oem[6] = '\0';
    //ConsolePrint("OEM = %s\n", oem);

    // Check version
    uint8_t revision = p[15];
    if (revision == 0)
    {
 //       ConsolePrint("Version 1\n");
//		kprintf("revision 0");
        uint32_t rsdtAddr = *(uint32_t *)(p + 16);
        AcpiParseRsdt((AcpiHeader *)(uintptr_t)rsdtAddr);
    }
/*    else if (revision == 2)
    {
        ConsolePrint("Version 2\n");

        uint32_t rsdtAddr = *(uint32_t *)(p + 16);
        uint64_t xsdtAddr = *(u64 *)(p + 24);

        if (xsdtAddr)
        {
            AcpiParseXsdt((AcpiHeader *)(uintptr_t)xsdtAddr);
        }
        else
        {
            AcpiParseRsdt((AcpiHeader *)(uintptr_t)rsdtAddr);
        }
    }
  */  else
    {
        kprintf("Unsupported ACPI version %d\n", revision);
    }

    return 1;
}
void AcpiInit()
{
    // TODO - Search Extended BIOS Area

    // Search main BIOS area below 1MB
    uint8_t *p = (uint8_t *)0x000e0000;
    uint8_t *end = (uint8_t *)0x000fffff;

    while (p < end)
    {
        uint64_t signature = *(uint64_t *)p;

        if (signature == 0x2052545020445352) // 'RSD PTR '
        {
           if (AcpiParseRsdp(p))
           {
			kprintf("\nSignature found\n");
               break;
            }
        }

        p += 16;
    }
}
int AcpiRemapIrq(int irq)
{
    AcpiMadt *madt = s_madt;

    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    while (p < end)
    {
     //   kprintf("%x %x\n", p, end);
		ApicHeader *header = (ApicHeader *)p;
        uint8_t type = header->type;
        uint8_t length = header->length;

        if (type == APIC_TYPE_INTERRUPT_OVERRIDE)
        {
            kprintf("\ntype found\n");
			ApicInterruptOverride *s = (ApicInterruptOverride *)p;

            if (s->source == irq)
            {
            kprintf("\nsource type found %x\n", s->interrupt);
                return s->interrupt;
            }
        }

        p += length;
    }

    return irq;
}/*
void ioapic_set_irq(uint8_t irq, uint64_t apic_id, uint8_t vector) {
    const uint32_t low_index = 0x10 + irq*2;
    const uint32_t high_index = 0x10 + irq*2 + 1;

    uint32_t high = IoApicIn((uint32_t *)high_index);
    // set APIC ID
    high &= ~0xff000000;
    high |= apic_id << 24;
    IoApicOut(high_index, high);

    uint32_t low = IoApicIn((uint32_t *)low_index);

    // unmask the IRQ
    low &= ~(1<<16);

    // set to physical delivery mode
    low &= ~(1<<11);

    // set to fixed delivery mode
    low &= ~0x700;

    // set delivery vector
    low &= ~0xff;
    low |= vector;

    IoApicOut(()low_index, low);
}
*/
void apicMain(void){

		idt_init();
		pic_init();
		pic_disable();
		kprintf("APIC val: %x", check_apic());
		kprintf("MSR val: %d", check_msr());
//		AcpiInit();
//		cpu_set_apic_base(0x25000);
//		IOAPIC_BASE = (uint64_t *)0xFEC00000;;
//		APIC_BASE = (uint64_t *)cpu_get_apic_base();
		kprintf("\n%x\n", APIC_BASE);
//		kprintf("\nTesting APIC! Local APIC revision: %x Max LVT entry: %x\n",apicread(APIC_BASE + LAPIC_VER)&&0xff, ((apicread(APIC_BASE + LAPIC_VER)>>16) && 0xff)+1);
		//AcpiInit();
		cpu_set_apic_base(0x25000);
		//IOAPIC_BASE = (uint32_t *)0x45000;
		APIC_BASE = (uint32_t *)cpu_get_apic_base();
		IOAPIC_BASE = (uint32_t *)0xFEC00000;
		kprintf("\n%x\n", APIC_BASE);
		init_map_virt_phys_addr((uint64_t)0xfec00000, (uint64_t)0xfec00000, 1);
		//init_map_virt_phys_addr(0x00000000fec00000, 0x00000000fec00000, 1024);
	//	idt_init();
//		apicwrite(APIC_BASE + 0xF0, apicread(APIC_BASE + 0xF0) | 0x100);
//		apicwrite(APIC_BASE + LAPIC_ERROR, 0x1F); /// 0x1F: temporary vector (all other bits: 0)
/*		apicwrite(APIC_BASE + LAPIC_TPR, 0);
		apicwrite(APIC_BASE + LAPIC_DFR, 0xffffffff);
		apicwrite(APIC_BASE + LAPIC_LDR, 0x1000000);
<<<<<<< HEAD
		apicwrite(APIC_BASE + LAPIC_SVR, 0x100|0x00);
=======
		apicwrite(APIC_BASE + LAPIC_SVR, 0x1FF);
>>>>>>> 34ee2afdec9040e9246fe0a3322f94837de14354
		IoApicInit();
		timer_init();
//IoApicSetEntry(IOAPIC_BASE, AcpiRemapIrq(0x00), 0x10);
//		__asm__ volatile("sti");
//kprintf("Passing: %x\n", APIC_BASE + LAPIC_DFR);
		kprintf("\n%x\n", apicread(APIC_BASE + LAPIC_DFR));
		kprintf("\n%x\n", apicread(APIC_BASE + LAPIC_LDR));
		kprintf("\n%x\n", apicread(APIC_BASE + LAPIC_SVR));

//		mask_init();
//		apicwrite(APIC_BASE + LAPIC_TIMER, 32 | 0x20000);
		apic_start_timer();
*/		
		
//		while(1)
//			kprintf("Current Val: %x\n", apicread(APIC_BASE + LAPIC_TCCR));
//		__asm__ volatile("int $0x20");
//*/
}
