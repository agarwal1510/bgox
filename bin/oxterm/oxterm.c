#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define COMM_LEN 50

int main(int argc, char *argv[], char *envp[]){
		
				char input[COMM_LEN] ;
				char cmd[COMM_LEN] ;
				char arg[COMM_LEN] ;
		while(1){
				for(int i = 0; i < COMM_LEN; i++){
					input[i] = '\0';
					cmd[i] = '\0';
					arg[i] = '\0';
				}
				gets(input);
				//read(0, input, COMM_LEN);
				if (input[0] == '\n')
					continue;
				else if (str_len(input) >= COMM_LEN){
					printf("\noxTerm: Careful with command lengths !\n");
					continue;
				}
				int delim = strfind_delim(input, 0);
				str_substr(input, 0, delim-1, cmd);
				str_substr(input, delim+1, str_len(input), arg);
				cmd[delim] = '\0';	
				arg[str_len(input)-delim-2] = '\0';
				printf("%s","");
				
				pid_t pid = 1;

				pid = fork();
				if (pid == 0){
					execvp(cmd, arg);
				}
				else{
						//		print(pids);
					yield();
					//printf("Ppid: %d\n", getppid());
					//while(1);
					/*char *filename = "nigga";
                                         file *fd = open((filename));
                                         if (fd != NULL) {
                                                 printf("success");
                                         } else {
                                                 printf("null");
                                         }
                                         char buffer[10];
                                         read_file(fd, (void *)buffer, 10);
                                         printf("B: %s", buffer);
                                         close(fd);*/
                                         uint64_t ret = opendir("bin/");
                                         read_dir(ret);
                                         closedir(1);
					wait();
					//printf("status:%d\n", status);
					//waitpid(0);
				}
		}
		//	waitpid(0);
		exit(1);
		return 0;
}
