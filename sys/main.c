#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
//#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pci.h>
#include <sys/apic.h>
#include <sys/mem.h>
#include <sys/ptmgr.h>
#include <sys/threads.h>
#include <sys/syscall.h>

uint64_t initial_stack[STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
//struct pcb *bootProcess;
DEFN_SYSCALL1(kprintf, 0, const char*);
//void thread1(){


//}
/*
void thread_handler(){
//	__asm__ volatile("int $0x21");
	
//	kprintf("Handler Thread called\n");
	
	while(1){}
//	return;
}*/
void myfunc(){
	while(1);
//__asm__ volatile("int $0x80");
//	syscall_kprintf("Nigga be in user mode");
}
void start(uint32_t *modulep, void *physbase, void *physfree)
{
		struct smap_t {
				uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  uint64_t last_mem_ptr = 0;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      last_mem_ptr = smap->base + smap->length;
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, last_mem_ptr);
    }
  }
  uint64_t num_pages = last_mem_ptr/0x1000;
  calculate_free_list(num_pages, (uint64_t)physfree);
  //uint64_t *ptr = kmalloc(8500);
  //kprintf("Kmalloc %p %d\n", ptr, ((struct page*)ptr)->block_size);
  //free(ptr);
  //uint64_t *ptr2 = kmalloc(8000);
  //kprintf("Lmalloc2 %p", ptr2);
  init_paging(KERNEL_VADDR + (uint64_t)physbase, (uint64_t)physbase, 800);
  kprintf("Page Tables Setup complete\n");
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
//  struct posix_header_ustar *tarfshead = (struct posix_header_ustar*)(&_binary_tarfs_start+512);
//  int temp = 0;
//  int size = octalToDecimal(atoi(tarfshead->size));

//  kprintf("size: %d", size);
  
//  if(size == 0)
//		  temp = temp + 512;
//  else
//		  temp +=  (size%512==0) ? size + 512 : size + (512 - size%512) + 512;
  //  tarfshead += sizeof(struct posix_header_ustar);
  //  tarfshead = (struct posix_header_ustar*)(tarfshead + 512);
//  kprintf("%s\n", tarfshead->name);
//  tarfshead = (struct posix_header_ustar*)(&_binary_tarfs_start+temp);
//  kprintf("%s\n", tarfshead->size);
//  apicMain();
//  find_ahci();
// switch_thread();
init_tarfs();
uint64_t p = opendir("lib/");
kprintf("%p", p);
read_dir(p);
kmain();
//syscall_init();
set_tss_rsp(initial_stack);

//TODO disable interrupts before this and renable after pushf using EFLAGS;
  __asm__ __volatile__("mov $0x23, %ax\n"
	"mov %ax, %ds\n"
	"mov %ax, %es\n"
	"mov %ax, %fs\n"
	"mov %ax, %gs\n"
	"mov %rsp, %rax\n"
	"push $0x23\n"
	"push %rax\n"
	"pushf\n"
	"push $0x1B\n"
	"push $1f\n"
	"iretq\n"
	"1:\n");

syscall_kprintf("Teri maa ki chut\n");

// while(1){}
//  switch_user_thread();
  while(1);
}
void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[STACK_SIZE-1])
  );
  init_gdt();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1){ __asm__ volatile ("hlt");}
}
