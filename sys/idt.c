#define MAX_IDT 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <stdlib.h>
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);
extern void isr0(void);
extern void keyboard_handler(void);
extern void timer_init(void);
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

void outb(int16_t port, uint8_t value){
	__asm__ ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void _x86_64_asm_lidt(struct idtr_t *idtr);

void irq_handler(void){
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
void setup_gate(int32_t num){
	uint64_t handler_addr = (uint64_t)isr0;
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
//	for(int i=0;i<=33;i++)
//		setup_gate(i);
	setup_gate(32);
//	unsigned long idt_address;
//	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	/* fill the IDT descriptor */
//	idt_address = (unsigned long)IDT ;
//	idt_ptr[0] = sizeof(IDT);
///	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0x0ffff) << 32);
//	idt_ptr[1] = idt_address ;
//
	_x86_64_asm_lidt(&idtr);
}

void kb_init(void){
	outb(0x21 , 0xFE);
}
/*void timer_init(){

		int frequency = 100;
		uint16_t divisor = 1193180/frequency;
		// Send the command byte.
		outb(0x43, 0x36);

		// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
		uint8_t l = (uint8_t)(divisor & 0xFF);
		uint8_t h = (uint8_t)(divisor>>8);

		// Send the frequency divisor.
		outb(0x40, l);
		outb(0x40, h);
}
*/void kmain(void){

	kprintf("%s", "In kmain");
	pic_init();
	idt_init();
	kb_init();
	timer_init();
// for(int i = 0;i < 1000000; i++) {
//	ticks++; 
//	  }
//	while(1);
}
