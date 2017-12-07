#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

DEFN_SYSCALL1(kill, 24, const char *);

int main (int argc, char *argv[], char *envp[]) {
	if (argc < 1)
		print("kill: insufficient arguments. Usage: kill -9 PID\n");
	else {
		syscall_kill(argv[0]);
	}
//	print("below kill");
	exit(1);
}
