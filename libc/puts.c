#include <stdio.h>
#include <sys/syscall.h>

DEFN_SYSCALL2(putc, 1, int, const char*);

int puts(const char *s)
{
	syscall_putc(1, s);
	return 1;
}
