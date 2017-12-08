#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/tarfs.h>

typedef struct registers
{
    uint64_t ds;                  // Data segment selector
    uint64_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint64_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint64_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;


void syscall_init();

#define DECL_SYSCALL0(fn) int syscall_##fn();
#define DECL_SYSCALL1(fn,p1) int syscall_##fn(p1);
#define DECL_SYSCALL2(fn,p1,p2) uint64_t syscall_##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) uint64_t syscall_##fn(p1,p2,p3);

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn() \
{ \
	int a; \
	__asm__ volatile("int $0x80" : "=a" (a) : "0" (num):"rcx", "rdx", "rsi", "rdi", "r8", "r9"); \
	return a; \
}

#define DEFN_SYSCALL1(fn, num, P1) \
int syscall_##fn(P1 p1) \
{ \
	uint64_t a; \
	__asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((uint64_t)p1):"rcx", "rdx", "rsi", "rdi", "r8", "r9"); \
	return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
uint64_t syscall_##fn(P1 p1, P2 p2) \
{ \
 uint64_t a; \
 __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((uint64_t)p1), "c" ((uint64_t)p2):"rdx","rsi","rdi","r12","r11","r10","r9","r8","r13","r14","r15"); \
		return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
uint64_t syscall_##fn(P1 p1, P2 p2, P3 p3) \
{ \
		uint64_t a; \
		__asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((uint64_t)p1), "c" ((uint64_t)p2), "d"((uint64_t)p3):"cc", "memory"); \
		return a; \
}

DECL_SYSCALL0(ls)
DECL_SYSCALL0(ps)
DECL_SYSCALL1(echo, const char*)
DECL_SYSCALL1(cat, const char *)
DECL_SYSCALL1(sleep, const char *)
DECL_SYSCALL1(exec, char*)
DECL_SYSCALL2(kprintf, int, const char*)
DECL_SYSCALL2(execvp, char *, char *)
DECL_SYSCALL3(read, int, char *, size_t)
DECL_SYSCALL3(write, int, uint64_t, size_t)

//////////////////////////////////////////

#define _syscall0(type,name) \
		type name() \
	{ \
			long __res; \
			__asm__ volatile ("int $0x80" \
							: "=a" (__res) \
							: "a" (__NR_##name) \
							: "memory"); \
			return (type) __res; \
	}
#define _syscall1(type,name,type1,arg1) \
	type name(type1 arg1) \
	{ \
			long __res; \
			__asm__ volatile ("int $0x80" \
							: "=a" (__res) \
							: "a" (__NR_##name),"b" ((long)(arg1)) \
							: "memory"); \
			return (type) __res; \
	}
#define _syscall2(type,name,type1,arg1,type2,arg2) \
		type name(type1 arg1,type2 arg2) \
{ \
		long __res; \
		__asm__ volatile (  "int $0x80" \
						: "=a" (__res) \
						: "a" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)) \
						: "memory"); \
		return (type)__res; \
}
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
		type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
		long __res; \
		__asm__ volatile (  "int $0x80" \
						: "=a" (__res) \
						: "a" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
						"d" ((long)(arg3)) \
						: "memory"); \
		return (type)__res; \
}
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3, type4, arg4) \
		type name(type1 arg1,type2 arg2,type3 arg3, type4, arg4) \
{ \
		long __res; \
		__asm__ volatile (  "int $0x80" \
						: "=a" (__res) \
						: "a" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
						"d" ((long)(arg3)), "s" ((long)(arg4)) \
						: "memory"); \
		return (type)__res; \
}

#endif
