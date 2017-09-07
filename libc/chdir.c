#include <sys/syscall.h>
#include <unistd.h>

#define __NR_chdir 12
_syscall1(int, chdir, const char *, path);
