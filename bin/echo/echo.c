#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL1(echo, 16, const char *);
int main(int argc, char *argv[], char *envp[]){
	if (argc < 1)
		print("echo: insufficient arguments. Usage: echo echo-string");
	else
		syscall_echo(argv[0]);
	while(1);
	exit(1);
}
