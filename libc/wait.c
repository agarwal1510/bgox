#include <sys/syscall.h>

DEFN_SYSCALL0(wait, 32);

pid_t wait() {
	int ret = syscall_wait();
	return ret;
}
