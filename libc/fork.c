#include <sys/syscall.h>
#include <unistd.h>

#define __NR_fork 2

_syscall0(pid_t, fork);
