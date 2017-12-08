#include <sys/syscall.h>
#include <sys/tarfs.h>

DEFN_SYSCALL1(close, 36, file *);

int close(file *fd) {
	int ret = syscall_close(fd);
	return ret;
}
