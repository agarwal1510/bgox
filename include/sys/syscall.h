#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/kprintf.h>
#include <sys/defs.h>

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
#define DECL_SYSCALL2(fn,p1,p2) int syscall_##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) int syscall_##fn(p1,p2,p3);

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn() \
{ \
	int a; \
	__asm__ volatile("int $0x80" : "=a" (a) : "0" (num)); \
	return a; \
}

#define DEFN_SYSCALL1(fn, num, P1) \
int syscall_##fn(P1 p1) \
{ \
	int a; \
	__asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((long)p1)); \
	return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
int syscall_##fn(P1 p1, P2 p2) \
{ \
 int a; \
 __asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((long)p1), "c" ((long)p2)); \
		return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
		int syscall_##fn(P1 p1, P2 p2, P3 p3) \
{ \
		int a; \
		__asm__ volatile("int $0x80" : "=a" (a) : "0" (num), "b" ((long)p1), "c" ((long)p2), "d"((long)p3)); \
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
DECL_SYSCALL3(read, int, char *, uint16_t)
//DECL_SYSCALL1(write_hex, const char*)

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
