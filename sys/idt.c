#define MAX_IDT 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define KB_DATA	0x60
#define KB_STATUS 0x64
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/portio.h>
#include <stdlib.h>
#include "kb_map.h"
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
	outb(0x20, 0x20);
	outb(0xa0, 0x20);
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
	outb(0x20, 0x20);
	outb(0xa0, 0x20);

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
			if (SHIFT_ON == 1)
			kprintf_at("%c", 148, 24, kbdus[key] - 32);
			else
			kprintf_at("%c", 148, 24, kbdus[key]);
		}
	}
}


void pic_init(void){ // SETUP Master and Slave PICS
    outb(0x20, 0x11);
    outb(0xA0, 0x11); 
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xff);
    outb(0xA1, 0xff);
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

void mask_init(void){
	outb(0x21 , 0xFC); //11111100
}

void kmain(void){

	pic_init();
	idt_init();
	mask_init();
	timer_init();
}
