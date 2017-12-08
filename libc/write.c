#include <sys/syscall.h>
#include <sys/defs.h>
#include <stdio.h>

DEFN_SYSCALL3(write, 1, int, uint64_t, size_t);

ssize_t write(int fd, char *s, size_t size)
{
	int bytes = syscall_write(1, (uint64_t)s, size);
	return bytes;
}

