#include <stdio.h>
#include <sys/syscall.h>
#include <sys/defs.h>

DEFN_SYSCALL3(readb, 2, int, char *, uint16_t);

int readb(int fd, char *s, uint16_t size)
{
	int bytes = syscall_readb(0, s, size);
	return bytes;
}
