#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL1(echo, 16, const char *);
int main(int argc, char *argv[], char *envp[]){
	if (argc < 1)
		print(2, "echo: Usage: echo echo-string\n");
	else
		syscall_echo(argv[0]);
	exit(1);
}
