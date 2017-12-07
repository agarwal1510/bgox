#include <sys/kprintf.h>
#include <sys/process.h>

void irq_0_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Invalid operation: Divide by zero\n");
	sys_exit(1);
}
void irq_1_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Debug exception raised\n");
}
void irq_3_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Debug: Breakpoint encountered\n");
}
void irq_4_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Operand overflow detected. Terminating Process\n");
	sys_exit(1);
}
void irq_5_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Exception: Bound range exceeded\n");
}
void irq_6_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Invalid opcode\n");
}
void irq_7_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: error: Device not available\n");
}
void irq_8_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Double Fault encountered. Terminating Process\n");
	sys_exit(1);
}
void irq_9_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	__asm__ volatile ("int $0xd"); // handle using GPF;
}
void irq_10_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Invalid TSS exception\n");
}
void irq_11_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Invalid Segment load performed. Terminating Process\n");
	sys_exit(1);
}
void irq_12_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Stack Seg Fault. Terminating Process\n");
}
void irq_16_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Floating point exception raised\n");
	sys_exit(1);
}
void irq_17_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: error: Unaligned memory reference performed. Terminating Process\n");
	sys_exit(1);
}
void irq_18_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Machine check: internal error detected\n");
}
void irq_19_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: Floating Point Exception raised\n");
}
void irq_20_handler(void){
//	outb(0x20, 0x20);
//	outb(0xa0, 0x20);
	kprintf("oxTerm: error: Virtualization Exception\n");
}
