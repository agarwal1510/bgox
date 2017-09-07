#include <sys/syscall.h>
#include <sys/wait.h>

#define __NR_waitpid 7
_syscall3(pid_t, waitpid, pid_t, pid, int*, status, int, options);
