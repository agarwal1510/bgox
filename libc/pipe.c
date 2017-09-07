#include <sys/syscall.h>
#include <unistd.h>

#define __NR_pipe 42
_syscall1(int, pipe, int*, pipefd);
