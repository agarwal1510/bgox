#include <stdio.h>
#include <sys/syscall.h>

DEFN_SYSCALL1(gets, 30, char *);

int gets(char *s) {
	int ret  = syscall_gets(s);
	return ret;
}
