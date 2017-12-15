#include <sys/syscall.h>

DEFN_SYSCALL2(execvp, 8, char *, char *);

int execvp(char *proc, char* arg) {
	int ret = syscall_execvp(proc, arg);
	return ret;
}
