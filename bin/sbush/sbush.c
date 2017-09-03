#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define COMM_LEN  100
#define BIN_LEN  10
#define ARG_LEN  20
#define ENV_LEN 100
#define PATH ""
#define PS1_DEFAULT "sbush> "
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
	return i;
}
int str_cmp(char* str1, char* str2){
	
	return (*str1 == *str2 && *str1 == '\0') ? 1 : (*str1 == *str2 ? str_cmp(++str1, ++str2) : -1);

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
		(getenv("PS1") == NULL) ? setenv("PS1", PS1_DEFAULT, 1) : setenv("PS1", getenv("PS1"), 1);
		char *PS1 = getenv("PS1");
		fputs(PS1, stdout);
		if (fgets(command, 100, stdin) != NULL){
			command[str_len(command)-1] = '\0';
			char *comm = command;
			char *args[COMM_LEN];
			int arg_ctr = comm_parser(&comm, args);
			args[arg_ctr] = NULL;
			
			if (str_cmp(args[0], "cd") > 0){
				if (chdir(args[1]) < 0)
					puts("sbush: error executing command: cd");
			}
			else if (str_cmp(args[0], "export") > 0){ //TODO Make export persistant by wrinting to file 
					//TODO Check valid string enclosed within ""
					int delim_idx = strfind_occurence(command, '=', 1);
					char env_var[BIN_LEN];
					str_substr(command, str_len(args[0])+1, delim_idx-1, env_var);
				if (str_cmp(env_var, "PS1") > 0){
					char env_val[ENV_LEN];
					str_substr(command, delim_idx+2, str_len(command)-2, env_val); //Null and New line were two extra chars
					setenv("PS1", env_val, 1);
				}
				else if (str_cmp(env_var, "PATH") > 0){
					printf("Exporting PATH");
				}
			}
			else{
				execvp(args[0], args);
				
				if(WIFEXITED(status)){
         			fputs("sbush: command not found: ", stdout);
					puts(args[0]);
				}
			}
			
		}
		else{
			puts("Error reading input from stdin");
		}
	}
  
  
  }
  return 0;
}
