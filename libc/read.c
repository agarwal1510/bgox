#include <sys/syscall.h>
#include <sys/defs.h>
#include <stdio.h>

DEFN_SYSCALL3(read, 2, int, char *, size_t);

ssize_t read(int fd, char *s, size_t size)
{
	int bytes = syscall_read(0, s, size);
	return bytes;
}

