#ifndef _ELF64_H
#define _ELF64_H
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/process.h>
#include <sys/threads.h>

#define EI_NIDENT 16

# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(2)  // 64-bit Architecture
# define EM_386		(0x3E)  // x86-64 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version
# define ELF_PROG_LOAD 1

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Lword;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Sword;
typedef uint64_t Elf64_Sxword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half    e_type;
  Elf64_Half    e_machine;
  Elf64_Word    e_version;
  Elf64_Addr    e_entry;
  Elf64_Off     e_phoff;
  Elf64_Off     e_shoff;
  Elf64_Word    e_flags;
  Elf64_Half    e_ehsize;
  Elf64_Half    e_phentsize;
  Elf64_Half    e_phnum;
  Elf64_Half    e_shentsize;
  Elf64_Half    e_shnum;
  Elf64_Half    e_shstrndx;
} Elf64_Ehdr;

enum Elf_Ident {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
	EI_CLASS	= 4, // Architecture (32/64)
	EI_DATA		= 5, // Byte Order
	EI_VERSION	= 6, // ELF Version
	EI_OSABI	= 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		= 9  // Padding
};

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};

typedef struct {
  Elf64_Word    p_type;
  Elf64_Word    p_flags;
  Elf64_Off     p_offset;
  Elf64_Addr    p_vaddr;
  Elf64_Addr    p_paddr;
  Elf64_Xword   p_filesz;
  Elf64_Xword   p_memsz;
  Elf64_Xword   p_align;
} Elf64_Phdr;

task_struct *elf_parse(uint64_t estart_addr, file *fileptr);

#endif
