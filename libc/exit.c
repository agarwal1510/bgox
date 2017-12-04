#include <sys/syscall.h>
#define __NR_exit 1

DEFN_SYSCALL1(exit, 10, uint64_t)

void exit(uint64_t status) {

	syscall_exit(status);
}
