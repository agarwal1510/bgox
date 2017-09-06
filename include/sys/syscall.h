#define _syscall1(type,name,type1,arg1) \
	type name(type1 arg1) \
	{ \
			long __res; \
			__asm__ volatile ("int $0x80" \
							: "=a" (__res) \
							: "0" (__NR_##name),"b" ((long)(arg1))); \
			return (type) __res; \
	}
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
		type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
		long __res; \
		__asm__ volatile (  "int $0x80" \
						: "=a" (__res) \
						: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
						"d" ((long)(arg3))); \
		return (type)__res; \
}

