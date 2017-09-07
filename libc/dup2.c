#include <unistd.h>
#include <sys/syscall.h>

#define __NR_dup2 63

_syscall2(int, dup2, int, oldfd, int, newfd);
