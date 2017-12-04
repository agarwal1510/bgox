#include <sys/process.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>
#include <sys/strings.h>
#include <sys/paging.h>

extern void isr128(void);
void switch_to(task_struct *next, task_struct *me);

ready_task *ready_queue = NULL;
ready_task *head = NULL;
ready_task *previous = NULL;

task_struct *get_running_task() {
	return head->process;
}

void add_to_task_list(task_struct *process) {
	
	if (ready_queue == NULL){
		ready_queue = (ready_task *)kmalloc(sizeof(ready_task));
		ready_queue->process = process;
		ready_queue->next = NULL;
		head = ready_queue;
		return;
	}
	ready_task *temp = ready_queue;
	while(ready_queue->next != NULL){
		ready_queue = ready_queue->next;
	}

	ready_task *new_task = (ready_task *)kmalloc(sizeof(ready_task));
	new_task->process = process;
	new_task->next = NULL;
	ready_queue->next  = new_task;
	ready_queue = temp;
}

void schedule() {
	if (head->next != NULL) {
		previous = head;
		head = head->next;
	} else {
		previous = head;
		head = ready_queue;			
	}
	if (head->process->pid > 0){ //Not Kernel
		set_tss_rsp(&head->process->kstack[511]);
//		kprintf("\npid > 0\n");
	}
	kprintf("Saving rsp: %p",&head->process->kstack[511]);
	kprintf("\nnext: %d me: %d", head->process->pid, previous->process->pid);
	//		if (make == 1)
	switch_to(head->process, previous->process);

}

void switch_to(task_struct *next, task_struct *me) {
	//kprintf("Nextrsp: %p\n", next->rsp);

/*	__asm__ __volatile__( "pushq %rax");
	__asm__ __volatile__( "pushq %rbx");
	__asm__ __volatile__( "pushq %rcx");
	__asm__ __volatile__( "pushq %rdx");
	__asm__ __volatile__( "pushq %rdi");
	__asm__ __volatile__( "pushq %rsi");
	__asm__ __volatile__( "pushq %rbp");
	__asm__ __volatile__( "pushq %r8");
	__asm__ __volatile__( "pushq %r9");
	__asm__ __volatile__( "pushq %r10");
	__asm__ __volatile__( "pushq %r11");
	__asm__ __volatile__( "pushq %r12");
	__asm__ __volatile__( "pushq %r13");
	__asm__ __volatile__( "pushq %r14");
	__asm__ __volatile__( "pushq %r15");

*/
	__asm__ volatile ("movq %%rsp, %0" : "=r"(me->rsp));
	__asm__ volatile ( "movq %0, %%cr3;"
	              :: "r" (next->cr3));
	
	__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));
//	if (next->pid == 2)
//		while(1);
//	kprintf("switch");
/*
	__asm__ __volatile__( "popq %r15");
	__asm__ __volatile__( "popq %r14");
	__asm__ __volatile__( "popq %r13");
	__asm__ __volatile__( "popq %r12");
	__asm__ __volatile__( "popq %r11");
	__asm__ __volatile__( "popq %r10");
	__asm__ __volatile__( "popq %r9");
	__asm__ __volatile__( "popq %r8");
	__asm__ __volatile__( "popq %rbp");
	__asm__ __volatile__( "popq %rsi");
	__asm__ __volatile__( "popq %rdi");
	__asm__ __volatile__( "popq %rdx");
	__asm__ __volatile__( "popq %rcx");
	__asm__ __volatile__( "popq %rbx");
	__asm__ __volatile__( "popq %rax");
*/	
//	__asm__ __volatile__("iretq");
}

void test_function() {
	kprintf("in test function");
	while(1);

}

uint64_t sys_fork() {
	kprintf("Fork called: %p", get_tss_rsp());
	task_struct *parent = get_running_task();
	task_struct *child = (task_struct *) kmalloc(sizeof(task_struct));
	kprintf("%p %d %p", child, parent->pid, parent);
	add_to_task_list(child);

	child->mm = (mm_struct *)((char *)(child+1));
	child->mm->mmap = NULL;
	child->pid = PID++;
	child->ppid = parent->pid;
	//memcpy(child->tname, parent->tname, str_len(parent->tname));
	
	struct page *pt_page = (struct page*) kmalloc(1);
	uint64_t *pml4a = (uint64_t *)(pt_page);
	for (uint64_t i = 0; i < PAGES_PER_PML4; i++) {
		pml4a[i] = 0;
	}

	pml4a[PAGES_PER_PML4 - 1] = (uint64_t)(ker_pml4_t[511]);
//	init_map_virt_phys_addr(0x0, 0x0, 24000, pml4a, 1);

//	uint64_t curr_rsp = 0;
	uint64_t *temp_kstack = kmalloc(1);
	uint64_t *temp_ustack = kmalloc(1);
	child->ustack = kmalloc_stack(1, pml4a);
	child->pml4 = (uint64_t)pml4a;
	child->cr3 = (uint64_t *)PADDR(pml4a);
//	kprintf("%p", child->ustack);
	for (uint64_t j = 0; j < 512; j++) {
		temp_kstack[j] = parent->kstack[j];
		temp_ustack[j] = parent->ustack[j];
//		kprintf("%p", parent->ustack[j]);
	}

//init_map_virt_phys_addr(0x0, 0x0, 32000, (uint64_t *)child->pml4, 1);
	__asm__ volatile ("movq %0, %%cr3;"::"r"(child->cr3));
	kprintf("cr3: %p", child->cr3);
	for (uint64_t j = 0; j < 512; j++) {
		child->ustack[j] = temp_ustack[j];
	}
//		kprintf("%p", parent->ustack[j]);
//	init_map_virt_phys_addr((uint64_t)child->ustack, PADDR(child->ustack), 1, pml4a, 1);
//	__asm__ volatile ("movq %0, %%cr3;"::"b"(parent->cr3));
//	uint64_t curr_rsp;
//	__asm__ volatile ("movq %%rsp, %0;":"=m"(curr_rsp));

	child->kstack[511] = 0x23;
	child->kstack[510] = (uint64_t)(&child->ustack[511]);
	child->kstack[509] = 0x200286;
	child->kstack[508] = 0x1b;
	child->kstack[507] = (uint64_t)(child->ustack[505]);    //(uint64_t) &test_function; //entry point-505

	child->kstack[506] = 0;
	child->kstack[505] = temp_kstack[504];
	child->kstack[504] = temp_kstack[503];
	child->kstack[503] = temp_kstack[502];
	child->kstack[502] = temp_kstack[501];
	child->kstack[501] = temp_kstack[500];
	child->kstack[500] = temp_kstack[499];
	child->kstack[499] = temp_kstack[498];
	child->kstack[498] = temp_kstack[497];
	child->kstack[497] = temp_kstack[496];
	child->kstack[496] = temp_kstack[495];
	child->kstack[495] = temp_kstack[494];
	child->kstack[494] = temp_kstack[493];
	child->kstack[493] = temp_kstack[492];
	child->kstack[492] = temp_kstack[491];
	child->kstack[491] = (uint64_t)(&isr128+29);
	
	child->rsp = &(child->kstack[491]);
//	child->rsp = &(child->kstack[491]);
//	kmccrintf("Child cr3: %p\n", child->cr3);
//	init_map_virt_phys_addr(0x0, 0x0, 32000, pml4a, 1);
	
	vm_area_struct *p_vma = parent->mm->mmap;
	vm_area_struct *child_vma;
	while (p_vma != NULL) {
		child_vma = vma_malloc(child->mm);
		child_vma->vm_start = p_vma->vm_start;
		child_vma->vm_end = p_vma->vm_end;
		child_vma->vm_mmsz = p_vma->vm_mmsz;
		child_vma->vm_next = p_vma->vm_next;
		child_vma->vm_file = p_vma->vm_file;
		child_vma->vm_flags = p_vma->vm_flags;
		child_vma->vm_pgoff = p_vma->vm_pgoff;
		kprintf("vma: %p %p", p_vma->vm_start, p_vma->vm_mmsz);
		__asm__ volatile ("movq %0, %%cr3;"::"b"(parent->cr3));
		region_alloc(child, p_vma->vm_start, p_vma->vm_mmsz);
		__asm__ volatile ("movq %0, %%cr3;"::"b"(child->cr3));

		p_vma = p_vma->vm_next;
	}
	
	__asm__ volatile ("movq %0, %%cr3;"::"b"(parent->cr3));


//	while(1);
//	__asm__ volatile ("call 1f\n"
//			"1: popq %0\n"
//			:"=r"(curr_rsp));
	//kprintf("RSPf: %p", curr_rsp);
	
	
//	if (get_running_task() == parent) {
		kprintf("parent : %p", temp_kstack[506]);	
		//child->kstack[507] = curr_rsp;
		return child->pid;
//	} else {
//		kprintf("child");
//		return 0;
//	}
	//return child->pid;;

}

/*
void switch_thread(){	

	process1 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr = &thread1;

	for(int i = 0; i < STACK_SIZE; i ++){
		process1->kstack[i] = 0;
	}

	process1->kstack[STACK_SIZE - 1] = (uint64_t)funcptr;
	process1->rip = (uint64_t)funcptr;
	process1->rsp = (uint64_t)(&(process1->kstack[STACK_SIZE - 1]));

	process2 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		process2->kstack[i] = 0;
	}
	kprintf("Stack bound: %p\n", (uint64_t)&process2->kstack[STACK_SIZE-1]);
	process2->kstack[STACK_SIZE - 1] = (uint64_t)funcptr2;
	process2->rip = (uint64_t)funcptr2;
	process2->rsp = (uint64_t)(&(process2->kstack[STACK_SIZE - 16]));

	add_to_ktask_list(process1);
	add_to_ktask_list(process2);
	//	schedule();
	//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
	thread1();
}

void switch_user_thread(){	

	u_process1 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr = &thread1;

	for(int i = 0; i < STACK_SIZE; i ++){
		u_process1->kstack[i] = 0;
	}

	u_process1->kstack[STACK_SIZE - 1] = 0x23;
	u_process1->kstack[STACK_SIZE - 2] = (uint64_t)(&(u_process1->kstack[STACK_SIZE - 1]));
	u_process1->kstack[STACK_SIZE - 3] = 0x200286; //rflags  
	u_process1->kstack[STACK_SIZE - 4] = 0x1b; //cs
	u_process1->kstack[STACK_SIZE - 5] = (uint64_t)funcptr;  //rip entry point scroll down to see the value 
	
	for(int i = 0; i < 15; i++){
		u_process1->kstack[STACK_SIZE - 6 - i] = i;	
	}
//	u_process1->rip = (uint64_t)funcptr;
	u_process1->rsp = (uint64_t)(&(u_process1->kstack[STACK_SIZE - 2]));



	u_process2 = (struct tcb *) kmalloc(sizeof(struct tcb));
	void *funcptr2 = &thread2;

	for(int i = 0; i < STACK_SIZE; i ++){
		u_process2->kstack[i] = 0;
	}

	u_process2->kstack[STACK_SIZE - 1] = 0x23;
	u_process2->kstack[STACK_SIZE - 2] = (uint64_t)(&(u_process2->kstack[STACK_SIZE - 1]));
	u_process2->kstack[STACK_SIZE - 3] = 0x200286; //rflags  
	u_process2->kstack[STACK_SIZE - 4] = 0x1b; //cs
	u_process2->kstack[STACK_SIZE - 5] = (uint64_t)funcptr2;  //rip entry point scroll down to see the value 
	
	for(int i = 0; i < 15; i++){
		u_process2->kstack[STACK_SIZE - 6 - i] = i;	
	}
	u_process2->rip = (uint64_t)funcptr2;
	u_process2->rsp = (uint64_t)(&(u_process2->kstack[STACK_SIZE - 20]));

	add_to_ktask_list(u_process1);
	add_to_ktask_list(u_process2);
		schedule();
	//	__asm__ volatile ("movq %0, %%rsp;"::"m"(process1->rsp));
//	thread1();
}*/
