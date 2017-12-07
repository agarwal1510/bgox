#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
DEFN_SYSCALL0(list_dir, 12);
int main(int argc, char *argv[], char *envp[]){
	syscall_list_dir();
	exit(1);
}
