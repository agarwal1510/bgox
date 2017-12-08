#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

int puts(const char *s)
{
	return write(1, s, str_len((char*)s));
}
