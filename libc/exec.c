#include <sys/syscall.h>

DEFN_SYSCALL1(exec, 8, char *);

int exec(char *proc) {
	int ret = syscall_exec(proc);
	return ret;
}
