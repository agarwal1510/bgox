#include <sys/syscall.h>
#define __NR_exit 1

_syscall1(void, exit, int, EXIT_STATUS);
void _exit(int status)
{
	exit(status);
}
