#include <sys/syscall.h>
#include <sys/wait.h>

#define __NR_waitpid 12
//_syscall3(pid_t, waitpid, pid_t, pid, int*, status, int, options);
DEFN_SYSCALL1(waitpid, 12, int);

pid_t waitpid(int pid) {
	int ret = syscall_waitpid(pid);
	return ret;
}
