#include <sys/process.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/gdt.h>
#include <sys/strings.h>
#include <sys/paging.h>
#include <sys/utils.h>

extern void isr128(void);
void switch_to(task_struct *next, task_struct *me, int first_switch);

ready_task *queue_head = NULL;
ready_task *running_task = NULL;
ready_task *previous = NULL;

task_struct *get_running_task() {
	return running_task->process;
}

void print_task_list() {
         kprintf("PID NAME STATUS\n");
          ready_task *temp = queue_head;
          char *status = NULL;
          while (temp != NULL) {
                  kprintf("%d %s", temp->process->pid, temp->process->tname);
                  if (temp->process->is_waiting > 0) {
                          status = "Waiting";
                 } else if (temp->process->is_sleeping == 1) {
                          status = "Sleeping";
                  } else {
                          status = "Running";
                  }
                  kprintf(" %s\n", status);
                  temp = temp->next;
          }
}

void dec_sleep_count() {
//	kprintf("dec ");
	ready_task *temp = queue_head;
	while (temp != NULL) {
		if (temp->process->is_sleeping == 1) {
		temp->process->sleep_time -= 1;
//		kprintf("RSP: %p", temp->process->rsp);
		if (temp->process->sleep_time == 0) {
			temp->process->is_sleeping = 0;
			//add_to_task_list(temp->process);
//			remove_from_sleeping_queue(temp->process);
		}
		}
		temp = temp->next;

	}
}

void delete_curr_from_task_list(){
	ready_task *temp = queue_head;
//	kprintf("RP: %d", running_task->process->pid);
	
	while (temp->next != running_task) {
		temp = temp->next;
	}

	if (running_task->next){
		temp->next = running_task->next;
		//running_task->next = NULL;
		running_task = NULL;
	}
	else{
//		kprintf("No next encounterd");
		temp->next = NULL;
		running_task = NULL;

	}
	task_count -= 1;
}

void delete_from_task_list(uint64_t pid) {
	ready_task *temp = queue_head;
	while (temp != NULL) {
		if (temp->process->pid == pid) {
			break;
		}
		temp = temp->next;
	}
	if (temp == NULL) {
		kprintf("Process with PID:%d not found\n", pid);
	} else {
		if (temp == previous) {
			temp = queue_head;
			while (temp->next != previous) {
				temp = temp->next;
			}
			if (previous->next) {
				temp->next = previous->next;
			} else {
				temp->next = NULL;
			}
			
		} else if (temp == queue_head){
			queue_head = temp->next;
		} else {
			ready_task *del = temp;
			temp = queue_head;
			while (temp->next != del) {
				temp = temp->next;
			}
			if (del->next) {
				temp->next = del->next;
			} else {
				temp->next = NULL;
			}
		}
	}
}

void add_to_task_list(task_struct *process) {
	
	if (queue_head == NULL){
		queue_head = (ready_task *)kmalloc(sizeof(ready_task));
		queue_head->process = process;
		queue_head->next = NULL;
		running_task = queue_head;
		task_count = 1;
		return;
	}
	ready_task *temp = queue_head;
	while(queue_head->next != NULL){
		queue_head = queue_head->next;
	}

	ready_task *new_task = (ready_task *)kmalloc(sizeof(ready_task));
	new_task->process = process;
	new_task->next = NULL;
	queue_head->next  = new_task;
	queue_head = temp;
	task_count += 1;
}

int is_child_done(uint64_t pid) {
	int present = 0;
	ready_task *temp = queue_head;
	while (temp != NULL) {
		if (temp->process->pid == pid) {
			present = 1;
			break;
		}
		temp = temp->next;
	}
	if (present == 0) {
		return 1;
	} else if (temp->process->is_sleeping == 0){
		return 1;
	}
	return 0;
}



void schedule(int first_switch) {
	if (running_task == NULL) {
//		kprintf("here");
		if (previous->next == NULL) {
			running_task = queue_head;
		} else {
			ready_task *temp = previous;
			while (temp->next != NULL 
				&& (temp->next->process->is_sleeping != 0
				|| temp->next->process->is_waiting > 0)) {
//				kprintf("%d ", temp->process->is_waiting);
			if (temp->process->is_waiting > 0 && (is_child_done(temp->process->is_waiting))) {
//				kprintf("Done ");
				temp->process->is_waiting = 0;
			}
				temp = temp->next; 
			}
//		if (temp->process->is_waiting > 0 && (is_child_done(temp->process->is_waiting))) {
//			temp->process->is_waiting = 0;
//		}
		//kprintf("id: %d", temp->process->pid);
		if (temp->next != NULL) {
			running_task = temp->next;
		} else {
			running_task = queue_head;			
		}
			//running_task = previous->next;
		}
	} else {
		ready_task *temp = running_task;
		while (temp->next != NULL 
			&& (temp->next->process->is_sleeping != 0
			|| temp->next->process->is_waiting > 0)) {

			if (temp->process->is_waiting > 0 && (is_child_done(temp->process->is_waiting))) {
//				kprintf("Done ");
				temp->process->is_waiting = 0;
			}
			temp = temp->next; 
		}
//		if (temp->process->is_waiting > 0 && (is_child_done(temp->process->is_waiting))) {
			//kprintf("Done");
//			temp->process->is_waiting = 0;
//		}

		if (temp->next != NULL) {
			previous = running_task;
			running_task = temp->next;
		} else {
			previous = running_task;
			running_task = queue_head;			
		}
	}

	set_tss_rsp(&running_task->process->kstack[511]);
//	if (count == 10) while(1);
//	kprintf("\nnext: %d me: %d %d", running_task->process->pid, previous->process->pid, first_switch);
	switch_to(running_task->process, previous->process, first_switch);

}


void switch_to(task_struct *next, task_struct *me, int first_switch) {
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
	if (first_switch == 0) {
		__asm__ volatile ("movq %%rsp, %0" : "=r"(me->rsp));
		__asm__ volatile ( "movq %0, %%cr3;":: "r" (next->cr3));
		__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));
	} else {
		__asm__ volatile ( "movq %0, %%cr3;":: "r" (next->cr3));
		__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));
		
	}

//	__asm__ volatile ( "movq %0, %%cr3;":: "r" (next->cr3));
//	__asm__ __volatile__ ("movq %0, %%rsp;"::"r"(next->rsp));
//	if (next->pid == 2)
	//	while(1);
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
//	kprintf("Fork called: %p", get_tss_rsp());
	task_struct *parent = get_running_task();
	task_struct *child = (task_struct *) kmalloc(sizeof(task_struct));
//	kprintf("%p %d %p", child, parent->pid, parent);

	child->mm = (mm_struct *)((char *)(child+1));
	child->mm->mmap = NULL;
	child->pid = PID++;
	child->ppid = parent->pid;
	//kprintf("Pid: %d %d", child->pid, child->ppid);
	memcpy(child->tname, parent->tname, str_len(parent->tname));
	
	add_to_task_list(child);
	
	struct page *pt_page = (struct page*) kmalloc(1);
	uint64_t *pml4a = (uint64_t *)(pt_page);
	for (uint64_t i = 0; i < PAGES_PER_PML4; i++) {
		pml4a[i] = 0;
	}

	pml4a[PAGES_PER_PML4 - 1] = (uint64_t)(ker_pml4_t[511]);
//	init_map_virt_phys_addr(0x0, 0x0, 24000, pml4a, 1);

//	uint64_t curr_rsp = 0;
	uint64_t *temp_kstack = kmalloc(1);
	child->pml4 = pml4a;
	uint64_t *temp_ustack = kmalloc(1);
//	child->ustack = kmalloc_stack(1, pml4a);
	child->cr3 = (uint64_t *)PADDR(pml4a);
	child->is_waiting = 0;
	child->sleep_time = 0;
	child->is_sleeping = 0;
//	kprintf("%p", child->ustack);
	for (uint64_t j = 0; j < 512; j++) {
		temp_kstack[j] = parent->kstack[j];
		temp_ustack[j] = parent->ustack[j];
		child->pml4[j] = parent->pml4[j];
//		kprintf("%p ", parent->kstack[j]);
	}
	child->ustack = kmalloc_stack(1, child->pml4);
//	uint64_t ptee = walk_page_table((uint64_t)parent->ustack);
//	kprintf("***DOOM: %p %p***", parent->ustack, ptee);

//	init_map_virt_phys_addr((uint64_t)child->ustack, (uint64_t)PADDR(temp_ustack), 1, pml4a, 1);
//	init_map_virt_phys_addr((uint64_t)parent->ustack, (uint64_t)ptee, 1, pml4a, 1);
	
//	static uint64_t bumpPtr=(uint64_t )(0xFFFFF0F080700000);
//	init_map_virt_phys_addr((uint64_t)parent->ustack, (uint64_t)((uint64_t)child->ustack - (uint64_t)bumpPtr), 1, pml4a, 1);
	
	__asm__ volatile ("movq %0, %%cr3;"::"r"(child->cr3));
//	kprintf("cr3: %p", child->cr3);
	for (uint64_t j = 0; j < 512; j++) {
		child->ustack[j] = temp_ustack[j];
		child->kstack[j] = temp_kstack[j];
//		kprintf("%p ", parent->kstack[j]);
//		kprintf("%p ", child->ustack[j]);
	}
	uint64_t i;
	for (i = 0; i < 512; i++) {
		if (temp_ustack[i] != 0) {
			break;
		}
	}
	uint64_t offset = 511 - i - 1;

//	init_map_virt_phys_addr((uint64_t)child->ustack, (uint64_t)PADDR(temp_ustack), 1, pml4a, 1);
//	init_map_virt_phys_addr((uint64_t)parent->ustack, (uint64_t)ptee, 1, pml4a, 1);

	child->kstack[511] = 0x23;
	child->kstack[510] = (uint64_t)(&child->ustack[511 - offset]);
//	child->kstack[510] = temp_kstack[506];
	child->kstack[509] = 0x200286;
	child->kstack[508] = 0x1b;
	//child->kstack[507] = (uint64_t)(child->ustack[505]);    //(uint64_t) &test_function; //entry point-505
	child->kstack[507] = temp_kstack[506];    //(uint64_t) &test_function; //entry point-505

	child->kstack[506] = 0;
//	child->kstack[505] = temp_kstack[504];
//	child->kstack[504] = temp_kstack[503];
//	child->kstack[503] = temp_kstack[502];
//	child->kstack[502] = temp_kstack[501];
//	child->kstack[501] = temp_kstack[500];
//	child->kstack[500] = temp_kstack[499];
//	child->kstack[499] = temp_kstack[498];
//	child->kstack[498] = temp_kstack[497];
//	child->kstack[497] = temp_kstack[496];
//	child->kstack[496] = temp_kstack[495];
//	child->kstack[495] = temp_kstack[494];
//	child->kstack[494] = temp_kstack[493];
//	child->kstack[493] = temp_kstack[492];
//	child->kstack[492] = temp_kstack[491];
	child->kstack[491] = (uint64_t)(&isr128+29);

	child->rsp = &(child->kstack[491]);
//	child->rsp = &(child->kstack[491]);
//	kmccrintf("Child cr3: %p\n", child->cr3);
//	init_map_virt_phys_addr(0x0, 0x0, 32000, pml4a, 1);
//	kprintf("errwwe");
	vm_area_struct *p_vma = parent->mm->mmap;
	vm_area_struct *child_vma;
	while (p_vma != NULL) {
//		kprintf("inside vma");
		child_vma = vma_malloc(child->mm);
		child_vma->vm_start = p_vma->vm_start;
		child_vma->vm_end = p_vma->vm_end;
		child_vma->vm_mmsz = p_vma->vm_mmsz;
		child_vma->vm_next = NULL;
		child_vma->vm_file = p_vma->vm_file;
		child_vma->vm_flags = p_vma->vm_flags;
		child_vma->vm_pgoff = p_vma->vm_pgoff;
//		kprintf("vma: %p %p", p_vma->vm_start, p_vma->vm_mmsz);
//		__asm__ volatile ("movq %0, %%cr3;"::"b"(parent->cr3));
		region_alloc(child, p_vma->vm_start, p_vma->vm_mmsz);
//		__asm__ volatile ("movq %0, %%cr3;"::"b"(child->cr3));
		for (uint64_t j = 0; j < p_vma->vm_mmsz; j++) {
			*((uint64_t *)(p_vma->vm_start + j)) = *((uint64_t *)((uint64_t)p_vma->vm_file + p_vma->vm_pgoff + j));
		}

		p_vma = p_vma->vm_next;
	}
//	kprintf("qwerty");	
	__asm__ volatile ("movq %0, %%cr3;"::"r"(parent->cr3));


//	while(1);
//	__asm__ volatile ("call 1f\n"
//			"1: popq %0\n"
//			:"=r"(curr_rsp));
	//kprintf("RSPf: %p", curr_rsp);
	
	
//	if (get_running_task() == parent) {
//		kprintf("parent : %p", temp_kstack[506]);	
		//child->kstack[507] = curr_rsp;
		return child->pid;
//	} else {
//		kprintf("child");
//		return 0;
//	}
	//return child->pid;;

}

void sys_exit(uint64_t status) {
//	schedule();
	//set_tss_rsp(&ready_queue->process->kstack[511]);
//	task_struct *current = get_running_task();
//	kprintf("%p", running_task->process->pid);
	delete_curr_from_task_list();
	//PID--;
//	running_task = queue_head;	
//	kprintf("\nnextd: %d med: %d", running_task->process->pid, previous->process->pid);
	schedule(1);
//	switch_to(running_task->process, previous->process, 1);

}

void sys_kill(uint64_t pid) {
//	kprintf("pid %d %d", pid, running_task->process->pid);	
	ready_task *temp = queue_head;
	if (running_task->process->ppid == pid) {
		kprintf("oxTerm: Session Terminated...\n");
		delete_from_task_list(pid);
		schedule(0);
		return;
	}
	while (temp != NULL) {
		if (temp->process->pid == pid && str_cmp(temp->process->tname, kernel) == 1) {
			kprintf("oxTerm: err: Can't kill kernel. You don't have privileges!\n");
			schedule(0);
			return;
		} else if (temp->process->pid == pid && temp == running_task) {
			delete_curr_from_task_list();
			schedule(1);
			return;
		}
		temp = temp->next;
	}
	delete_from_task_list(pid);
	schedule(0);
}

uint64_t sys_waitpid(uint64_t pid, uint64_t is_bg) {
	if (is_bg == 1) return 0;
	task_struct *current = get_running_task();
	task_struct *child;
	uint64_t ppid = current->pid;
	//int pid_found = 0;
	ready_task *temp = queue_head;
//	kprintf("PPid: %d %d", ppid, pid);
	if (pid < 0 || pid == 0) {
		while (temp != NULL) {
//			kprintf(" %d ", temp->process->ppid);
			if (temp->process->ppid == ppid && temp->process->is_bg != 1) {
				//kprintf("child found");
				child = temp->process;
				current->is_waiting = child->pid;
				schedule(0);
					//previous = running_task;
					//running_task = queue_head;
					//switch_to(running_task->process, previous->process, 1);
				return 0;
			}
			temp = temp->next;
		}
	} else {
		while (temp != NULL) {
			if (temp->process->pid == pid) {
				child = temp->process;
					current->is_waiting = child->pid;
					//previous = running_task;
					//running_task = queue_head;
					//switch_to(running_task->process, previous->process, 1);
					schedule(0);
					return 0;
			}
			temp = temp->next;
		}
	}
	//current->is_waiting = 0;
	return 0;
}

void sys_sleep(int time) {
	task_struct *current = get_running_task();
	current->sleep_time = time;
	current->is_sleeping = 1;
//	__asm__ volatile ("movq %%rsp, %0" : "=r"(current->rsp));
//	add_to_sleeping_queue(current);
//	delete_curr_from_task_list();
//	kprintf("sleep called");
	schedule(0);
}

