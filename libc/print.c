#include <stdio.h>
#include <sys/syscall.h>

DEFN_SYSCALL1(kprintf, 0, const char*);

void print(const char *s)
{
	syscall_kprintf(s);
}
