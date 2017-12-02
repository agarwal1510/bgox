#include <sys/syscall.h>
//#include <unistd.h>

//_syscall0(pid_t, fork);
DEFN_SYSCALL0(fork, 4);

pid_t fork() {
	pid_t pid = syscall_fork();
	return pid;
}
