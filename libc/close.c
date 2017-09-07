#include <sys/syscall.h>
#include <unistd.h>

#define __NR_close 6

_syscall1(int, close, int, fd);
