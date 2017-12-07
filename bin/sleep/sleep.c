#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

DEFN_SYSCALL1(sleep, 22, const char *);
int main(int argc, char *argv[], char *envp[]){
	
	if (argc < 1)
		print("sleep: Usage: sleep time-in-seconds\n");
	else {
		syscall_sleep(argv[0]);
	}
	exit(1);
}
