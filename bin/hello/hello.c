#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

		char input[COMM_LEN] = {0};
		char cmd[COMM_LEN];
//		char pets[10];
		while(1){
				read(0, input, COMM_LEN);
				print(input);
				str_substr(input, 0, str_len(input)-1, cmd);
				char split_cmd[2][COMM_LEN];
				split(cmd, split_cmd);
				print(split_cmd[0]);
//				itoa(ctr, pets, 10);
//				print(pets);


				pid_t pid = 22;

				pid = fork();

				if (pid == 0){
						print("Child here pid 0\n");
						print("wewewew");
						while(1);
//						execvp(cmd, "usr/userfile");
				}
				else{
						//		print(pids);
						print("Parent Here. Yielding now\n");
						yield();
						while(1);
						print("below yield");
				}
				for(int i = 0; i < COMM_LEN; i++){
						input[i] = 0;
						cmd[i] = 0;
				}
		}
		//	waitpid(0);
		exit(1);
		return 0;
}
