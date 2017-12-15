#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

DEFN_SYSCALL1(sleep, 22, const char *);
int main(int argc, char *argv[], char *envp[]){
	
	if (argc < 1){
		char *msg = "sleep: Usage: sleep time-in-seconds\n";
		write(2, msg, str_len(msg));
	}
	else {
		syscall_sleep(argv[0]);
	}
	exit(1);
}
