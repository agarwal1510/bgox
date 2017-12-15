#include <sys/syscall.h>
#include <sys/wait.h>

#define __NR_waitpid 20
//_syscall3(pid_t, waitpid, pid_t, pid, int*, status, int, options);
DEFN_SYSCALL2(waitpid, 20, int, int);

pid_t waitpid(int pid, int is_bg) {
	int ret = syscall_waitpid(pid, is_bg);
	return ret;
}
