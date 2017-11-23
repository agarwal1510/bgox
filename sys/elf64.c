#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

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

void elf_parse(uint64_t addr){
	Elf64_Ehdr *elfhdr = (Elf64_Ehdr *)addr;
	if (elf_check_supported(elfhdr) == true){
		kprintf("ELF file supported by machine\n");
	}
		kprintf("Now load the Program headers into a new page and start executing\n");
}
