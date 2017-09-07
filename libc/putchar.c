#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#define __NR_write 4

_syscall3(ssize_t, write, int, fd, const void*, buf, size_t, bytes );
int putchar(int c)
{
	int ret = write(1, &c, 1);
  // write character to stdout
  return ret;
}
