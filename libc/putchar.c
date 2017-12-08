#include <stdio.h>
#include <sys/syscall.h>

DEFN_SYSCALL2(kprintf, 1, int, const char*);

int putchar(int s)
{

	syscall_kprintf(1, (char*)&s);
	return 1;
}
