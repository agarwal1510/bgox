#include <sys/syscall.h>
#include <unistd.h>

DEFN_SYSCALL0(getppid, 28);

pid_t getppid(void) {
	int ret = syscall_getppid();
	return ret;
}
