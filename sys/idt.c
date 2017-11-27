#define MAX_IDT 256
#define INTERRUPT_GATE 0x8E
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define KB_DATA	0x60
#define KB_STATUS 0x64
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/portio.h>
#include <stdlib.h>
#include <sys/strings.h>
#include <sys/utils.h>
#include "kb_map.h"

extern void load_idt(unsigned long *idt_ptr);
extern void isr0(void);
extern void isr1(void);
extern void isr128(void);
extern void isr14(void);
extern void isr13(void);

void timer_init();

extern unsigned char kbdus[128];

static char char_buf[1024];
static volatile int buf_idx = 0;
static volatile int new_line = 0;

int SHIFT_ON = 0;
int CTRL_ON = 0;
/*struct gate_str{
  uint16_t offset_low;     // offset bits 0...15
  uint16_t selector;       // a code segment selector in GDT or LDT 16...31
  uint8_t zero;           // unused set to 0, set at 96...127
  uint8_t type_attr;       // type and attributes 40...43, Gate Type 0...3
  uint16_t offset_mid;  // offset bits 16..31, set at 48...63
  uint32_t offset_high;  // offset bits 16..31, set at 48...63
  uint32_t ist;           // unused set to 0, set at 96...127
  }  __attribute__((packed));
 */
struct gate_str
{
	uint16_t offset_low;
	uint16_t selector;
	unsigned ist : 3;
	unsigned reserved0 : 5;
	unsigned type_attr : 4;
	unsigned zero : 1;
	unsigned dpl : 2;
	unsigned p : 1;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved1;
} __attribute__((packed));

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


void page_fault_handler(void) {
	uint64_t pf_addr;
	__asm__ volatile ("movq %%cr2, %0":"=r"(pf_addr));
	kprintf("Page Fault address: %p\n", pf_addr);
	//__asm__ volatile ("hlt;");
		while(1);
}

void general_protection_fault_handler(void) {
	uint64_t pf_addr;
	__asm__ volatile ("movq %%cr2, %0":"=r"(pf_addr));
	kprintf("GPF Handler: %p\n", pf_addr);
	__asm__ volatile ("hlt;");
	while(1);
}

void irq_timer_handler(void){
	outb(0x20, 0x20);
	outb(0xa0, 0x20);
	if (ticks % 100 == 0){
		int new_sec = ticks/100;
		if (new_sec != sec){
			sec = new_sec;
			kprintf_boott(">Time Since Boot: ", sec);
		}
	}
	ticks++;
}

void syscall_handler(void) {

	uint64_t syscall_num = 0;
	uint64_t buf, third, fourth, fifth;
	//Save register values
	__asm__ volatile("movq %%rax, %0;"
			"movq %%rbx, %1;"
			"movq %%rcx, %2;"
			"movq %%rdx, %3;"
			"movq %%rdx, %4;"
			: "=g"(syscall_num),"=g"(buf), "=g"(third), "=g"(fourth), "=g"(fifth)
			:
			:"rax", "rbx", "rdx", "rsi","rcx"
		      );  
	
	if (syscall_num == 1){ // Write
		kprintf("%s", buf);
	}
	
	else if (syscall_num == 2){
				__asm__("sti");
				char *buf_cpy = (char *)third;
				int line = 0, idx = 0;
				while(line == 0 && idx < fourth){
				//poll
					line = new_line;
					idx = buf_idx;
				}
				memcpy(buf_cpy, char_buf, str_len(char_buf));
	}
/*	
	if (syscall_num == 2) { //Fork
	
	} else if (syscall_num == 3) { //Read

	} else if (syscall_num == 4) { //Write
		kprintf("%s", buf);
	}
*/
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
		if (key == 0x1c){
			new_line = 1;
			char_buf[buf_idx++] = '\n';
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
				
			char_buf[buf_idx++] = kbdus[key];
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
void setup_gate(int32_t num, uint64_t handler_addr, unsigned dpl){
	IDT[num].offset_low = (handler_addr & 0xFFFF);
	IDT[num].offset_mid = ((handler_addr >> 16) & 0xFFFF);
	IDT[num].offset_high = ((handler_addr >> 32) & 0xFFFFFFFF);
	IDT[num].selector = 0x08;
	IDT[num].type_attr = 0x0e;
	IDT[num].zero = 0x0;
	IDT[num].ist = 0x0;
	IDT[num].reserved0 = 0;
	IDT[num].p = 1;
	IDT[num].dpl = dpl;

}
void idt_init(void)
{
	setup_gate(13, (uint64_t)isr13, 0);
	setup_gate(14, (uint64_t)isr14, 0);
	setup_gate(32, (uint64_t)isr0, 0);
	setup_gate(33, (uint64_t)isr1, 0);
	setup_gate(128, (uint64_t)isr128, 3);
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
void timer_init(){
 
	uint32_t divisor = 1193180;
   outb(0x43, 0x36);

   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

   outb(0x40, l);
   outb(0x40, h);
}
