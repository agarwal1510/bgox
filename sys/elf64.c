#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/mem.h>
#include <sys/process.h>
#include <sys/memutils.h>
#include <sys/tarfs.h>
#include <sys/strings.h>
#include <sys/paging.h>
#include <sys/threads.h>
extern void isr128(void);

int elf_check_file(Elf64_Ehdr *elfhdr){

	if(!elfhdr)
		return false;
	if(elfhdr->e_ident[0] != 0x7F) {
		kprintf("ELF Header EI_MAG0 incorrect.\n");
		return false;
	}
	if(elfhdr->e_ident[1] != 'E') {
		kprintf("ELF Header EI_MAG1 incorrect.\n");
		return false;
	}
	if(elfhdr->e_ident[2] != 'L') {
		kprintf("ELF Header EI_MAG2 incorrect.\n");
		return false;
	}
	if(elfhdr->e_ident[3] != 'F') {
		kprintf("ELF Header EI_MAG3 incorrect.\n");
		return false;
	}
	kprintf("Valid ELF Header found\n");
	return true;
}

int elf_check_supported(Elf64_Ehdr *hdr) {

	if(!elf_check_file(hdr)) {
		kprintf("Invalid ELF File.\n");
		return false;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		kprintf("Unsupported ELF File Class.\n");
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		kprintf("Unsupported ELF File byte order.\n");
		return false;
	}
	if(hdr->e_machine != EM_386) {
		kprintf("Unsupported ELF File target.\n");
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		kprintf("Unsupported ELF File version.\n");
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		kprintf("Unsupported ELF File type.\n");
		return false;
	}
	return true;
}

void test_function() {
	kprintf("Test");
	while(1);
}

task_struct *elf_run_bin(Elf64_Ehdr *elfhdr, file *fileptr){

	struct page *process_page = (struct page *)kmalloc(1);

	task_struct *pcb = (task_struct *)kmalloc(sizeof(task_struct));

	//	ready_queue[num_processes++] = pcb;

	//	pcb->mm = (mm_struct *)((char *)(pcb + 1)); //WHY ?
	//	pcb->mm->count = 0;
	//	pcb->mm->mmap = NULL;

	//pcb->pid = PID++;

	//	memcpy(pcb->tname, fileptr->name, str_len(fileptr->name));

	uint64_t *pml4a=(uint64_t *)(process_page);//


	for(uint64_t i=0; i < PAGES_PER_PML4; i++) 
		pml4a[i] = 0;

	kprintf("%p %p", ker_pml4_t, ker_cr3);

	init_map_virt_phys_addr(0x0, 0x0, 24000, pml4a, 1);

	pml4a[PAGES_PER_PML4 - 1] = (uint64_t)(ker_pml4_t[511]);
	pcb->pml4 = (uint64_t)pml4a;
	pcb->cr3 = (uint64_t *)PADDR(pml4a);
	pcb->ustack = kmalloc(1);
	init_map_virt_phys_addr((uint64_t)pcb->ustack, PADDR(pcb->ustack), 1, pml4a, 1);
	//pcb->ustack = kmalloc_user((pml4e_t *)pcb->pml4e,STACK_SIZE);
	__asm__ volatile ( "movq %0, %%cr3;"
			:: "r" (pcb->cr3));


	kprintf("pcb->cr3=%p",pcb->cr3);


	pcb->kstack[506] = 1; pcb->kstack[505] = 2;  pcb->kstack[504] = 3;  pcb->kstack[503] = 4;
	pcb->kstack[502] = 5; pcb->kstack[501] = 6;  pcb->kstack[500] = 7;  pcb->kstack[499] = 8;
	pcb->kstack[498] = 9; pcb->kstack[497] = 10; pcb->kstack[496] = 11; pcb->kstack[495] = 12; 
	pcb->kstack[494] = 13; pcb->kstack[493] = 14; pcb->kstack[492] = 15; 

	pcb->kstack[491] = (uint64_t)(&isr128+29); 

	//pcb->rsp = (&pcb->kstack[493]);
	pcb->rsp = &(pcb->kstack[491]);

	pcb->kstack[511] = 0x23 ;                               
	pcb->kstack[510]=(uint64_t)(&pcb->ustack[511]);

	//   print("stack=%x",pcb->kstack[510]);
	pcb->kstack[509] = 0x246;                                            
	pcb->kstack[508] = 0x1b ;

	// LOAD ICODE HERE



	////////////////////////////////////////////////////////////////////////////////////




	//	for(int i = 1; i <= 15; i++)
	//		pcb->kstack[PAGES_PER_PML4 - 5 - i] = i;

	//	pcb->kstack[4080] = (uint64_t)(&isr32+34); 
	//	pcb->rsp = &(pcb->kstack[4080]);
	//
	//	 pcb->kstack[STACK_SIZE - 1] = 0x23 ;
	//	 pcb->kstack[STACK_SIZE - 2]=(uint64_t)(&pcb->pml4[STACK_SIZE - 1]);
	//	 pcb->kstack[STACK_SIZE - 3] = 0x246;
	//	 pcb->kstack[STACK_SIZE - 4] = 0x1b ;  


	Elf64_Phdr *phdr = (Elf64_Phdr *)(elfhdr + elfhdr->e_phoff);
	Elf64_Phdr *eph = phdr + elfhdr->e_phnum;
	kprintf("PHR: %p %p %d", phdr, eph, elfhdr->e_phnum);
	for(; phdr < eph; phdr++){
		kprintf("\nPH type: %d", phdr->p_type);
		if (phdr->p_type == ELF_PROG_LOAD) {

			if (phdr->p_filesz > phdr->p_memsz)
				kprintf("Wrong size in elf binary\n");

			kprintf("\n teesttt\n"); 

			//load_user_cr3(pcb->cr3);

			region_alloc(pcb, phdr->p_vaddr, phdr->p_memsz);
			memcpy((char*) phdr->p_vaddr, (void *) elfhdr + phdr->p_offset, phdr->p_filesz);

			if (phdr->p_filesz < phdr->p_memsz)
				memset((char*) phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);

			struct vm_area_struct *vm;
			vm = vma_malloc(pcb->mm);
			vm->vm_start = phdr->p_vaddr;
			vm->vm_end = vm->vm_start + phdr->p_memsz;
			vm->vm_mmsz = phdr->p_memsz;
			vm->vm_next = NULL;
			vm->vm_file =(uint64_t)elfhdr;
			vm->vm_flags = phdr->p_flags;
			vm->vm_pgoff = phdr->p_offset;  
		}
	}             	

	//pcb->entry = elfhdr->e_entry;
	pcb->entry = (uint64_t) &test_function;
	
	pcb->kstack[507] = (uint64_t)pcb->entry; 

	__asm__ volatile("movq %0, %%cr3":: "b"(ker_cr3));
	return pcb;
}

task_struct *elf_parse(uint64_t addr, file *fileptr){
	Elf64_Ehdr *elfhdr = (Elf64_Ehdr *)addr;
	if (elf_check_supported(elfhdr) == true){
		kprintf("ELF file supported by machine\n");
	}
	kprintf("Now load the Program headers into a new page and start executing\n");
	return elf_run_bin(elfhdr, fileptr);
}
