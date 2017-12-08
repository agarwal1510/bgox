#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
DEFN_SYSCALL1(echo, 16, const char *);
int main(int argc, char *argv[], char *envp[]){
	if (argc < 1){
		char *msg = "echo: Usage: echo echo-string\n";
		write(2, msg, str_len(msg));
	}
	else
		syscall_echo(argv[0]);
	exit(1);
}
