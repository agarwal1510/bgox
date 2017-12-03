#include <sys/syscall.h>
#include <stdio.h>

//_syscall0(pid_t, fork);
DEFN_SYSCALL0(fork, 4);

pid_t fork() {
///	while(1);
	pid_t pid = syscall_fork();
	print("fork return\n");
	return pid;
}
