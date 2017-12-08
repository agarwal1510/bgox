#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
DEFN_SYSCALL0(clear, 26);
int main(int argc, char *argv[], char *envp[]){
	syscall_clear();
	exit(1);
}
