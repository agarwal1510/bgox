#include <sys/syscall.h>

DEFN_SYSCALL0(getpid, 26);

uint32_t getpid(void) {
	uint32_t ret = syscall_getpid();
	return ret;
//	uint64_t ret = __syscall0(26);
//	return ret;
}
