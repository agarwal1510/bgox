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
#include <sys/process.h>
#include <sys/paging.h>
#include <sys/gdt.h>
#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/elf64.h>
#include <sys/mem.h>
#include <sys/env.h>
#include "kb_map.h"
extern int X;
extern int Y;
extern void load_idt(unsigned long *idt_ptr);
extern void isr0(void);
extern void isr1(void);
extern void isr128(void);
extern void isr14(void);
extern void isr13(void);
extern void pusha(void);
extern void popa(void);

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


void page_fault_handler(uint64_t err_code, uint64_t err_rip) {
	uint64_t pf_addr, curr_cr3;
	__asm__ volatile ("movq %%cr2, %0":"=r"(pf_addr));
	__asm__ volatile ("movq %%cr3, %0":"=r"(curr_cr3));
	kprintf("handler");
	kprintf("Page Fault address: %p %p %d\n", pf_addr, err_rip, err_code);
	walk_page_table(pf_addr);
	while(1);
	if (err_code >= 4){
//		struct page *pp = (struct page *)kmalloc(1);
//		kprintf("page addr: %p", pp);
//		memcpy((void*)pp, (void *)get_starting_page(pf_addr), 512);
//		init_map_virt_phys_addr(pf_addr, (uint64_t)pp, 1, (uint64_t *)VADDR(curr_cr3), 1);
		
		init_map_virt_phys_addr(pf_addr, (uint64_t)PADDR(pf_addr), 1, (uint64_t *)VADDR(curr_cr3), 1);
		//while(1);
		walk_page_table(pf_addr);
//:		walkpage_table((uint64_t)pp);
		while(1);
		__asm__ volatile ("movq %0, %%cr3"::"r"(curr_cr3));
//		while(1);
//	handle_page_fault(pf_addr, err_code);
	}
	//__asm__ volatile ("hlt;");
//		while(1);
}

void general_protection_fault_handler(uint64_t err_code, uint64_t err_rip) {
	uint64_t pf_addr;
	__asm__ volatile ("movq %%cr2, %0":"=r"(pf_addr));
	kprintf("GPF Handler: %p %d %p\n", pf_addr, err_code, err_rip);
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
		dec_sleep_count();
	}
	ticks++;
}

void syscall_handler(void) {

	uint64_t syscall_num = 0;
	uint64_t buf, third, fourth;
	//Save register values
	__asm__ volatile("movq %%rax, %0;"
			"movq %%rbx, %1;"
			"movq %%rcx, %2;"
			"movq %%rdx, %3;"
			: "=g"(syscall_num),"=g"(buf), "=g"(third), "=g"(fourth)
			:
			:"rax", "rsi","rcx", "rdx"
		      );  
	
	if (syscall_num == 1){ // Write
		kprintf("%s", buf);
	}
	else if (syscall_num == 2){
				__asm__("sti");
				kprintf(PS1);
				char *buf_cpy = (char *)third;
				int line = 0, idx = 0;
				while(line == 0 && idx < fourth){
				//poll
					line = new_line;
					idx = buf_idx;
				}
				memcpy(buf_cpy, char_buf, str_len(char_buf));
				memset(char_buf, 0, 1024);
				buf_idx = 0;
				new_line = 0;
//				__asm__ volatile("movq %0, %%rax;"::"r"(str_len(char_buf)));
	}
	else if (syscall_num == 4) {
		sys_fork();
	}
	else if (syscall_num == 6){
		schedule(0);
	}
	else if (syscall_num == 7){
//		kprintf("exec called for %s\n", buf);
		file* fd = open((char *)buf);
		if (fd == NULL)
			kprintf("exec error: Command not found");
		else{
			task_struct *parent = get_running_task();
			PID--; // Since pcb_exec increases PID by one on assignment
			char *argv1 = "myargs";
			char *argv[1] = {argv1};
			task_struct *pcb_exec = elf_parse(fd->addr+512,(file *)fd->addr, 0, argv);
			pcb_exec->pid = parent->pid;
			pcb_exec->ppid = parent->ppid;
			kprintf("Pid: %d %d", parent->pid, parent->ppid);
			delete_curr_from_task_list();
			add_to_task_list(pcb_exec);
			kprintf("name %s", pcb_exec->tname);
			schedule(1);
		}
	}
	else if (syscall_num == 8){
//		kprintf("execvp called for %s\n", buf);
		
		char cmd[50];
		str_concat(PATH, (char*)buf, cmd);
		file* fd = open((char *)cmd);
		if (fd == NULL){
			kprintf("oxTerm error: Command %s not found\n", buf);
			sys_exit(1);
		}
		else{
			task_struct *parent = get_running_task();
			PID--; // Since pcb_exec increases PID by one on assignment
			char argument[50];
			memcpy(argument, (char*)third, str_len((char*)third));
			char *argv[1] = {(char*)argument};
			task_struct *pcb_exec = elf_parse(fd->addr+512,(file *)fd->addr, 1, argv);
			pcb_exec->pid = parent->pid;
			pcb_exec->ppid = parent->ppid;
//			kprintf("%d", parent->pid);
			delete_curr_from_task_list();
			add_to_task_list(pcb_exec);
//			kprintf("name %s", pcb_exec->tname);
			schedule(1);
		}
	} 
	else if (syscall_num == 10) {
		//while(1);
//		kprintf("Exit called");
		sys_exit(buf);
	} 
	else if (syscall_num == 12){
		list_dir();
	}
	else if (syscall_num == 14){
			char *filename = (char *)buf;
			file *fd = open(filename);
			if (fd != NULL){
					int bytes = 80;
					int bread = 2048;
					char readbuf[bytes];
					while(bread >= bytes){
							bread = read(fd, readbuf, bytes);
							kprintf("%s", readbuf);
					}
					close(fd);
			}
	}
	else if (syscall_num == 16){
		kprintf("%s\n", buf); //Echo
	}
	else if (syscall_num == 18){
		kprintf("This is a process"); //ps
	}
	else if (syscall_num == 20) {
//		kprintf("waitId: %d", buf);
		sys_waitpid(buf);
	} else if (syscall_num == 22) {
		kprintf("sleep time: %d", atoi((char*)buf));
		sys_sleep(atoi((char*)buf));
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
//		kprintf("%p\n", key);
//		return;
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
			kprintf_at("%c", '^');
			return;
		}
		else if (key == 0x9d){
			CTRL_ON = 0;
			SHIFT_ON = 0;
			return;
		}
		else if (key == 0x0e){
			if (buf_idx > 0){
				char_buf[buf_idx] = 0;
				X -= 2;
				kprintf(" ");
				X -= 2;
				buf_idx--;
			}
			move_csr();
			return;
		
		}
		if (key == 0x1c){
			new_line = 1;
			kprintf("\n");
			char_buf[buf_idx++] = '\n';
			return;
		}
		if (key < 0x52){ // Keys over SSH
//			if (CTRL_ON == 0)
//				kprintf("%c", ' ');
			if (SHIFT_ON == 1){
				switch (key){
					case 2:
						kprintf("%c", '!');
						break;
					case 3:
						kprintf("%c", '@');
						break;
					case 4:
						kprintf("%c", '#');
						break;
					case 5:
						kprintf("%c", '$');
						break;
					case 6:
						kprintf("%c", '%');
						break;
					case 7:
						kprintf("%c",  '^');
						break;
					case 8:
						kprintf("%c", '&');
						break;
					case 9:
						kprintf("%c", '*');
						break;
					case 10:
						kprintf("%c", '(');
						break;
					case 11:
						kprintf("%c", ')');
						break;
					case 12:
						kprintf("%c", '_');
						break;
					case 13:
						kprintf("%c", '+');
						break;
					case 26:
						kprintf("%c", '{');
						break;
					case 27:
						kprintf("%c", '}');
						break;
					case 39:
						kprintf("%c", ':');
						break;
					case 40:
						kprintf("%c", '"');
						break;
					case 41:
						kprintf("%c", '~');
						break;
					case 43:
						kprintf("%c", '|');
						break;
					case 51:
						kprintf("%c", '<');
						break;
					case 52:
						kprintf("%c", '>');
						break;
					case 53:
						kprintf("%c", '?');
						break;
					default:
						kprintf("%c", kbdus[key] - 32);	
						break;
				}
			}
			else
				kprintf("%c", kbdus[key]);
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

//		outb(0x3D4, 0x0A);  // Disable ugly cursors
//		outb(0x3D5, 0x20);
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
