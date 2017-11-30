#include <sys/syscall.h>
#include <unistd.h>

#define __NR_fork 4

_syscall0(pid_t, fork);
