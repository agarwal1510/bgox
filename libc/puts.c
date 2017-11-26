#include <stdio.h>
#include <sys/syscall.h>
#define __NR_write 4

//DEFN_SYSCALL1(kprintf, 0, const char*);

_syscall3(ssize_t, write, int, fd, const void*, buf, size_t, bytes );
int puts(const char *s)
{
 int ret = write(1, s, sizeof(s)-1);
	return ret;
}
