#include <fcntl.h>
#include <sys/syscall.h>

#define __NR_open 5

_syscall2(int, open, const char *, pathname, int, flags);
