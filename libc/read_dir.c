#include <sys/syscall.h>
#include <sys/tarfs.h>

DEFN_SYSCALL1(read_dir, 42, uint64_t);

uint64_t read_dir(uint64_t buf) {
	uint64_t ret = syscall_read_dir(buf);
	return ret;
}
