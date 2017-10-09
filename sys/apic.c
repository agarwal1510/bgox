#define MAX_IDT 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define KB_DATA	0x60
#define KB_STATUS 0x64
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/portio.h>
//#include <stdlib.h>
#include "kb_map.h"
#define CPUID_FLAG_APIC 1 << 9
#define CPUID_FLAG_MSR 1 << 5
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800
extern void load_idt(unsigned long *idt_ptr);
extern void isr0(void);
extern void isr1(void);
extern void timer_init(void);
extern unsigned char kbdus[128];
int SHIFT_ON = 0;
int CTRL_ON = 0;

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

void irq_timer_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	if (ticks % 18 == 0){
		int new_sec = ticks/100;
		if (new_sec != sec){
			sec = new_sec;
			kprintf_boott(">Time Since Boot: ", sec);
		}
	}
	ticks++;
}

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
	setup_gate(32, (uint64_t)isr0);
	setup_gate(33, (uint64_t)isr1);
	_x86_64_asm_lidt(&idtr);
}
/*
void mask_init(void){
	outb(0x21 , 0xFC); //11111100
}
*/
void apicMain(void){

	kprintf("APIC val: %x", check_apic());
	kprintf("MSR val: %d", check_msr());
	cpu_set_apic_base(cpu_get_apic_base());
	kprintf("\n%x\n", cpu_get_apic_base());
	outl(0xF0, inl(0xF0) | 0x1FF);
	idt_init();
	outl(cpu_get_apic_base() | 0x320, 32 | cpu_get_apic_base() | 0x20000);
}
