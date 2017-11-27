#include <stdio.h>
#include <sys/syscall.h>
#include <sys/defs.h>

DEFN_SYSCALL3(read, 2, int, char *, uint16_t);

int read(int fd, char *s, uint16_t size)
{
	syscall_read(0, s, size);
	return 0;
}
