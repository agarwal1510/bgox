#include <unistd.h>
#include <sys/syscall.h>

#define __NR_read 3

_syscall3(ssize_t, read, int, fd, void *, buf, size_t, count);
