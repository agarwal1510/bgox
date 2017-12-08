#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL1(cat, 14, const char *);
int main(int argc, char *argv[], char *envp[]){
	
	if (argc < 1){
		char *msg = "cat: Usage: cat filename\n";
		write(2, msg, str_len(msg));
	}
	else{
		syscall_cat(argv[0]);
	}
	exit(1);
}
