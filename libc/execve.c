#include <sys/syscall.h>
#include <unistd.h>

#define __NR_execve 1
_syscall3(int, execve, const char*, file, char **const, argv, char **const, envp);
