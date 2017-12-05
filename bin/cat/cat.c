#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL1(cat, 14, const char *);
int main(int argc, char *argv[], char *envp[]){
	if (argc < 1)
		print("cat: insufficient arguments. Usage: cat filename");
	else
		syscall_cat(argv[0]);
	while(1);
	exit(1);
}
