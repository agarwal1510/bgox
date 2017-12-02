#include <sys/syscall.h>

DEFN_SYSCALL0(schedule, 6);

void yield() {
	syscall_schedule();
}
