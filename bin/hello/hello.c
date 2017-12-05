#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/process.h>
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
//    reverse(str, i);
 
    return str;
}

int main(int argc, char *argv[], char *envp[]){
//	print("Running hello now");
//	while(1);
//	char buf[1024] = {0};
//	for(int i = 0; i < 2; i++){
//		print("Tfsdfsdfsdfs Maa ki chut");	
//	}
//	char command[100] = "command";
	char *PS1 = "user@bgox $>";
	print (PS1);
//	read(0, command, 10);
	char pids[10];
//	read(0, s, 10);
//	print(s);
//	itoa(argc, pids, 10);
//	print(pids);
//	print("pids before");
	print(argv[0]);
//	while(1);
	pid_t pid = 22;
/*	if ((pid = fork()) != -1) {
		print("Inside");
		itoa(pid, pids, 10);
		print(pids);
//		print(&pid);
	} */
	pid = fork();
	print("hello");
	if (pid == 0){
		print("Child here pid 0\n");
//		print(command);
		while(1);
//	sleep(100);
//		execvp("bin/ls");
	}
	else{
		itoa(pid, pids, 10);
//		print(pids);
//		print(command);
		print("Parent Here. Yielding now\n");
//		waitpid(0);
		yield();
//		while(1);
		print("below yield");
	}
//	waitpid(0);
	exit(1);
	return 0;
}
