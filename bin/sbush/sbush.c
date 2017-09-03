#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define COMM_LEN  100
#define BIN_LEN  10
#define ARG_LEN  20

void str_cpy(char *to_str, char *frm_str){
	int i=0;
	for(i=0;frm_str[i] != '\0'; i++){
		to_str[i] = frm_str[i];
	}
	to_str[i] = '\0';
}
int str_len(char *str){
	int i=0;
	for(i=0;str[i] != '\0';i++){
		// spin
	}
	return i-1;
}
int strfind_occurence(char *str, char query, int occr){
	int find_ctr = 0;
	int i=0;
	for(i=0; str[i] != '\0'; i++){
		if (str[i] == query)
			find_ctr++;
		if (find_ctr == occr)
			break;
	}
	return i; // Returns found index or else null index;
}
int strfind_delim(char *str, int frm){
	int i=0;
	for(i=frm+1; str[i] != '\0'; i++){
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
			break;
	}
	return str[i] == '\0' ? i-1 : i;
}
void str_substr(char *str, int from, int to, char *out_str){
	int index = 0;
	for(int i = from; i <= to; i++){
		out_str[index] = str[i];
		index++;
	}
	out_str[index] = '\0';
}

int split(char *str, char out[][COMM_LEN]){
	int prev_ptr = 0;
	int i=0;
	int arg_ctr = 0;
	for(i=0; str[i] != '\0'; i++){
		if (str[i] == '\t' || str[i] == ' '){
			str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
			prev_ptr = i+1;
		}
	}
	str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
	return arg_ctr;
}

int comm_parser(char **comm, char *argv[COMM_LEN]){
	
	char args[ARG_LEN][COMM_LEN];
	char *comms = *comm;
	int arg_ctr = split(comms, args);
	*comm = args[0];
	for(int i=0; i < arg_ctr; i++){
		argv[i] = args[i] + '\0';
	}
	return arg_ctr;
}

int main(int argc, char *argv[], char *envp[]) {
  
  int pid, status;
  char command[COMM_LEN] = {'\0'};

  while(1){
  	pid = fork();
	if (pid != 0)
		waitpid(pid, &status, WUNTRACED);
	else{
		fputs("sbush> ", stdout);
		if (fgets(command, 100, stdin) != NULL){
			command[str_len(command)] = '\0';
			char *comm = command;
			char *args[COMM_LEN];
			int arg_ctr = comm_parser(&comm, args);
			args[arg_ctr] = NULL;
			execvp(args[0], args);
			
			if(WIFEXITED(status))
         		fputs("sbush: command not found: ", stdout);
				puts(args[0]);
		}
		else{
			puts("Error reading input from stdin");
		}
	}
  
  
  }
  return 0;
}
