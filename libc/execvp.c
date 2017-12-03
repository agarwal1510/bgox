#include <sys/syscall.h>

DEFN_SYSCALL1(execvp, 8, char *);

int execvp(char* file) {
	int ret = syscall_execvp(file);
	return ret;
}
