#include <sys/syscall.h>
#include <sys/tarfs.h>

DEFN_SYSCALL1(open, 34, char *)

file *open(char *filename) {
	uint64_t ret = syscall_open(filename);
	return (file *)ret;
}

