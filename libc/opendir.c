#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/tarfs.h>

DEFN_SYSCALL1(opendir, 40, char *);

uint64_t opendir(char *buf) {
	uint64_t ret = syscall_opendir(buf);
	return ret;
}
