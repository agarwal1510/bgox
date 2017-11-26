#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
//#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pci.h>
#include <sys/apic.h>
#include <sys/mem.h>
#include <sys/paging.h>
//#include <sys/threads.h>
#include <sys/process.h>
#include <sys/syscall.h>
#include <sys/elf64.h>

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
  kprintf("Num pages: %d", num_pages);
  calculate_free_list(num_pages, (uint64_t)physfree);
  //uint64_t *ptr = kmalloc(8500);
  //kprintf("Kmalloc %p %d\n", ptr, ((struct page*)ptr)->block_size);
  //free(ptr);
  //uint64_t *ptr2 = kmalloc(8000);
  //kprintf("Lmalloc2 %p", ptr2);
  init_paging(KERNEL_VADDR + (uint64_t)physbase, (uint64_t)physbase, num_pages);
  kprintf("Page Tables Setup complete\n");
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

  task_struct *pcb_boot = (task_struct *)kmalloc(sizeof(task_struct));  //kernel 
  pcb_boot->pml4 =(uint64_t)ker_pml4_t;  // kernel's page table   
  pcb_boot->cr3 = ker_cr3; // kernel's page table   
  pcb_boot->pid = 0;  // I'm kernel init process  so pid 0  
  //  pcb_boot->kstack = (uint64_t *)initial_stack; //my stack is already created by prof :)  
//  __asm__ volatile ("movq %%rsp, %0" : "=m"(pcb_boot->rsp));
  kprintf("\nEntry: %p\n", pcb_boot->rsp);
  add_to_task_list(pcb_boot);
  //  apicMain();
  //  find_ahci();
  // switch_thread();
  kmain();

  init_tarfs();


  uint64_t p = opendir("bin/");
  //kprintf("%p", p);
  read_dir(p);
  file* fd = open("bin/hello");
  //kprintf("handle for %s %p\n", fd->name, fd->addr);
  //char buf[100] = {0};

  task_struct *pcb_hello = elf_parse(fd->addr+512,(file *)fd->addr);
//  kprintf("\nEntry: %p\n", pcb_hello->rsp);
  add_to_task_list(pcb_hello);

  //if (read(fd, buf, 15) > 0){
  //	kprintf("buffer read: %s", buf);
  //}


  //uint64_t add = (uint64_t) 0xffffffff8eeeeeee;
  //struct page *pa = (struct page *)add;
  //kprintf("%d", pa->block_size);

  //syscall_init();
  set_tss_rsp(initial_stack);

  schedule();

  //TODO disable interrupts before this and renable after pushf using EFLAGS;
  /*  __asm__ __volatile__("mov $0x23, %ax\n"
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
   */
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
