#include <sys/syscall.h>

DEFN_SYSCALL1(sleep, 14, uint64_t)

void sleep(uint64_t time) {
	syscall_sleep(time);
}