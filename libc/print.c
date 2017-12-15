#include <stdio.h>
#include <sys/syscall.h>

DEFN_SYSCALL2(kprintf, 1, int, const char*);

void print(int stream, const char *s)
{
	syscall_kprintf(stream, s);
	return;
}
