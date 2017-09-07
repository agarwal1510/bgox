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
