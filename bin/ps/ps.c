#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL0(ps, 18);
int main(int argc, char *argv[], char *envp[]){
	syscall_ps();
	while(1);
	exit(1);
}
