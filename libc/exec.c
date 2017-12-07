#include <sys/syscall.h>
#include <stdio.h>

DEFN_SYSCALL1(exec, 7, char *);

int exec(char *proc) {
	print("EXec");
	int ret = syscall_exec(proc);
	return ret;
}
