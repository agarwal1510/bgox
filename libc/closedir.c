#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/defs.h>

DEFN_SYSCALL1(closedir, 44, uint64_t);

uint64_t closedir(uint64_t buf) {
	uint64_t ret = syscall_closedir(buf);
	return ret;
}
