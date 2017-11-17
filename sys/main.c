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

uint64_t initial_stack[STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
struct pcb *bootProcess;
//void thread1(){


//}
void thread_handler(){
	kprintf("Handler Thread called\n");
	while(1){}
//	return;
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
  init_paging(KERNEL_VADDR + (uint64_t)physbase, (uint64_t)physbase, 800);
  kprintf("Page Tables Setup complete\n");
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

<<<<<<< HEAD
//i  struct pcb *bootProcess = (struct pcb*)kmalloc(sizeof(struct pcb));
   
//  for(int i = 0; i < STACK_SIZE; i++){
//	bootProcess->kstack[i] = initial_stack[i];
//  }
//  uint64_t sp;
//  __asm__ volatile ("movq %%rsp, %0" : "=m"(sp));
//  kprintf("%p %p\n", bootProcess->kstack[STACK_SIZE-123], (uint64_t)(initial_stack[STACK_SIZE-123]));
//  kprintf("%p %p\n", sp, (uint64_t)&(initial_stack[STACK_SIZE-1]));
//  kprintf("%d", ((uint64_t)&(initial_stack[STACK_SIZE-1]) - sp));
//  __asm__ volatile (
//    "movq %0, %%rsp;"
//    :
//    :"r"(&bootProcess[STACK_SIZE-80])
//    :
//  );
//  set_tss_rsp(&bootProcess[STACK_SIZE-80]);
//  uint64_t sp2;
//  __asm__ volatile ("movq %%rsp, %0" : "=m"(sp2));
//  kprintf("%p %p\n", sp, (uint64_t)&(bootProcess[STACK_SIZE]));
  /*
  struct pcb *thread2 = (struct pcb*)kmalloc(sizeof(struct pcb));
  for(int i = 0; i < STACK_SIZE; i++){
  	thread2->kstack[i] = i;
  }
  void *funcptr = &thread_handler;
  thread2->kstack[STACK_SIZE-1] = (uint64_t)funcptr;
  thread2->rsp = (uint64_t)&(thread2->kstack[STACK_SIZE-5]);
  
  add_to_task_list(thread2);
  
  kprintf("%p %p", thread2->rsp, bootProcess->rsp);
  schedule();
*/
=======
//  apicMain();
//  find_ahci();
	switch_thread();
	//thread1();
>>>>>>> 0336ef48cb9ebf0b51e47d24a4f3c3647113aea5
//	thread2();
  switch_thread();
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
