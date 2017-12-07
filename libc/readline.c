#include <stdio.h>
#include <sys/syscall.h>
#include <sys/defs.h>
#include <sys/tarfs.h>

DEFN_SYSCALL3(readl, 3, file *, char *, uint16_t);

int readl(file *fd, char *s, uint16_t size)
{
	int bytes = syscall_readl(fd, s, size);
	return bytes;
}
