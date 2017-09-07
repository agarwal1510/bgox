#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
//#include <signal.h>
#include <unistd.h>
#define WNOHANG 1
#define WUNTRACED 2
#define COMM_LEN  100
#define BIN_LEN  10
#define ARG_LEN  20
#define ENV_LEN 1000
#define PATH_LEN 1000
#define PS1_DEFAULT "sbush> "
char ENV_PATH[PATH_LEN] = {-1};
char ENV_PS1[PATH_LEN] = {-1};
char ENV_HOME[PATH_LEN] = {-1};
void sig_handler(int signal){
		puts("\n+ done\n");

}
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
int str_contains(char *str, char *query){
		int j=0, i=0;
		int startIdx = -1, found = 0;
		for(i=0;str[i] != '\0';i++){
				if (str[i] == query[j]){
						j++;
						startIdx = i;
						if (j == str_len(query)){
								found = 1;
								break;
						}
				}
				else if (startIdx >= 0){
						j = 0;
						startIdx = -1;
				}
		}

		return (found == 1) ?  startIdx - str_len(query) + 1: -1; // Start Index of query substring
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
int split_delim(char *str, char delim, char out[][COMM_LEN]){
	int prev_ptr = 0;
	int i = 0;
	int arg_ctr = 0;
	for(i=0;str[i] != '\0';i++){
		if (str[i] == delim){
			str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
			prev_ptr = i+1;
		}
	}
	str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
	return arg_ctr;

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

void str_concat(char *prev, char *current, char *after, char *dest){
		int i=0;
		int j=0;
		int k=0;
		for(i = 0;prev[i] != '\0';i++){
				dest[i] = prev[i];
		}
		dest[i] = '\0';
		for(j = 0;current[j] != '\0';j++){
				dest[j + i] = current[j];
		}
		dest[i+j] = '\0';
		for(k = 0;after[k] != '\0';k++){
				dest[j + i + k] = after[k];
		}
		dest[i+j+k] = '\0';
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
void set_env(char *env_var, char env_val[]){
	if (str_cmp(env_var, "PATH") > 0){
		str_cpy(ENV_PATH, env_val);
	}
	else if (str_cmp(env_var, "PS1") > 0){
		str_cpy(ENV_PS1, env_val);
	}
	else if (str_cmp(env_var, "HOME") > 0){
		str_cpy(ENV_HOME, env_val);
	}
}
char* get_env(char *env_var){
	if (str_cmp(env_var, "PATH") > 0){
		return ENV_PATH;
	}
	else if (str_cmp(env_var, "PS1") > 0){
		return ENV_PS1;
	}
	else if (str_cmp(env_var, "HOME") > 0){
		return ENV_HOME;
	}
	return NULL;
}
void export_var(char *env_val, char *env_var){
		char deref_var[ENV_LEN];
		str_concat("$", env_var,"", deref_var);
		int idx = str_contains(env_val, deref_var);
		if (idx < 0){
				set_env(env_var, env_val);
				return;		
		}
		char temp[ENV_LEN];
		str_cpy(temp, env_val);
		char *current = get_env(env_var);
		while(idx >= 0){
				char prev[PATH_LEN];
				str_substr(temp, 0, idx-1, prev);
				char after[PATH_LEN];
				str_substr(temp, idx+str_len(env_var)+1, str_len(temp)-1, after);
				str_concat(prev, current, after, temp);
				set_env(env_var, temp);
				idx = str_contains(temp, deref_var);
		}
}

int exec_custom(char command[], char *args[], int arg_ctr){
		if (str_cmp(args[0], "cd") > 0){
				char path[PATH_LEN];
				str_cpy(path, args[1]);
				if (!args[1]){
					if (chdir(get_env("HOME")) < 0)
							puts("sbush: error executing command: cd");	
				}
				else if (chdir(path) < 0)
							puts("sbush: error executing command: cd");
				return 1;
		}
		else if (str_cmp(args[0], "export") > 0){ //TODO Make export persistant by wrinting to file 
				//TODO Check valid string enclosed within ""
				int delim_idx = strfind_occurence(command, '=', 1);
				char env_var[BIN_LEN];
				str_substr(command, str_len(args[0])+1, delim_idx-1, env_var);
				char env_val[ENV_LEN];
				str_substr(command, delim_idx+2, str_len(command)-2, env_val); //Null and New line were two extra chars
				if (str_cmp(env_var, "PS1") > 0){
						export_var(env_val, "PS1");
				}
				else if (str_cmp(env_var, "PATH") > 0){
						export_var(env_val, "PATH");
				}
				return 1;
		}
		return 0;
}
int exec_vp(char *path, char *args[], int BACKGROUND){
	char paths[ENV_LEN][COMM_LEN];
	int pid, status;
	int ctr = split_delim(ENV_PATH, ':', paths);
	str_cpy(paths[ctr], args[0]);
	ctr++;
	char clean_pref[COMM_LEN];
	str_cpy(clean_pref, paths[0]);
	int ret = -1;
	str_substr(clean_pref, 5, str_len(paths[0]) - 1, paths[0]);
	for(int i=0;i<ctr;i++){
			char path[PATH_LEN];
			char temp_path[PATH_LEN]; 
			if (i != ctr - 1){
					if (paths[i][str_len(paths[i]) - 1] == '/')
							paths[i][str_len(paths[i]) - 1] = '\0';
					str_concat(paths[i], "/", args[0], path);
					str_cpy(temp_path, args[0]);
					str_cpy(args[0], path);
			}
			pid = fork();
			if (pid != 0){
					if (BACKGROUND > 0){
						//	signal(SIGCHLD, SIG_IGN);
							if (waitpid(pid, &status, WNOHANG) == -1){
									continue;
							}
							else{
									ret = 1;
									break;
							}
					}
					else{
							if (waitpid(pid, &status, 0) == -1){
									continue;
							}
							else{
									ret = 1;
									break;
							}
					}
			}
			else{
					char *env[] = {ENV_PATH, ENV_PS1, ENV_HOME};
					execve(args[0], args, env);
			}
			if (i != ctr-1)
				str_cpy(args[0], temp_path);

	}
	return ret;	

}
void pipe_comm(char *args[][COMM_LEN], int ctr){

//		int pfd[ctr-1][2];
		int pid;
		int pfd[2];
			if (pipe(pfd) < 0){
				puts("Error piping commands");
			}
		/*
		for(int i=0;i<ctr-1;i++){
			if (pipe(pfd[i]) < 0){
				puts("Error piping commands");
			}
		}

		for(int i=0;i<ctr;i++){
		
			if ((pid = fork()) == 0){
				if (i != 0)
					dup2(pfd[i/2][0], 0);
				if (i < ctr-1)
					dup2(pfd[i/2][1], 1);
				if (i == 0)
					close(pfd[i/2][0]);
				if (i == ctr - 1)
					close(pfd[i/2][1]);
				execvp(args[i][0], args[i]);
			}
				else{
					if (i == 0)
					waitpid(pid, NULL, WUNTRACED);
				//	close(pfd[i][0]);
				//	close(pfd[i][1]);
				}
		} */
		if (fork() == 0){

				dup2(pfd[0], 0);
				close(pfd[1]);
				// TODO diff between execvp and execlp
				exec_vp(args[1][0], args[1], -1);
				puts("Could not create pipe");
	//			perror("could not do it");
		}
		else if ((pid = fork()) == 0){
						dup2(pfd[1], 1);
						close(pfd[0]);
						exec_vp(args[0][0], args[0], -1);
						puts("Could not create pipe");
					//	perror("could not do first");
		}
		else{
				waitpid(pid, NULL, WUNTRACED);				
				close(pfd[0]);
				close(pfd[1]);
		}
		return;
}
int main(int argc, char *argv[], char *envp[]) {

		char command[COMM_LEN] = {'\0'};
		while(*envp++){
				if (*envp == NULL)
						break;
				if (str_contains(*envp, "PATH=") >= 0)
						str_cpy(ENV_PATH, *envp);
				else if (str_contains(*envp, "PS1=") >= 0)
						str_cpy(ENV_PS1, *envp);
				else if (str_contains(*envp, "HOME=") >= 0)
						str_cpy(ENV_HOME, *envp);
		}
		if (argc > 1){
				char *const fileName = argv[1];
				char command[COMM_LEN];
				int line = 0, i=0;
				int file = open(fileName, O_RDONLY); 
				while (read(file, &command[i], 1) == 1) {
		//		str_cpy(command, "ls -la");
		//		while (command != NULL) {
						if (command[i] == '\n' || command[i] == 0x0) {
								command[i] = 0;
								line++;
								if (line > 1){
										char *args[COMM_LEN];
										char *comm = command;
										int arg_ctr = comm_parser(&comm, args);
										args[arg_ctr] = NULL;
										int BACKGROUND = str_cmp(args[arg_ctr-1], "&");
										if (BACKGROUND > 0){
												args[arg_ctr - 1] = NULL;
												arg_ctr--;
										}
										if (exec_custom(command, args, arg_ctr) == 0){
												if (exec_vp(args[0], args, BACKGROUND) < 0){
														char err[100];
														str_concat("sbush: error command not found: ",args[0], "", err); 
														puts(err);
												}
										}

								}
								i = 0;
								continue;
						}
						i++;
				}
				close(file);
				}
				else{
						((get_env("PS1"))[0] == -1) ? set_env("PS1", PS1_DEFAULT) : set_env("PS1", get_env("PS1"));
						char *PS1 = get_env("PS1");
						int ret = write(1, PS1, sizeof(PS1)-1);
						if (ret < 0)
								puts("sbush: Error writing to STDOUT");
						int i = 0;
						while (read(0, &command[i], 1) == 1) {
								if (command[i] == '\n' || command[i] == 0x0) {
										command[i] = 0;
										i = 0;
								if (str_cmp(command, "\n") > 0)
										continue;
								char *args[COMM_LEN];
								char *comm = command;

								int arg_ctr = comm_parser(&comm, args);
								args[arg_ctr] = NULL;
								int BACKGROUND = str_cmp(args[arg_ctr-1], "&");
								int PIPED = str_contains(command, "|");
								if (PIPED >= 0){
										char out_delim[COMM_LEN][COMM_LEN];
										int ctr = split_delim(command, '|', out_delim);
										//				printf("%d", ctr);
										char *args[ctr][COMM_LEN];
							//			args[0][0] = "ls";
						//				args[0][1] = "-la";
					//					args[0][2] = NULL;
				//						args[1][0] = "grep";
			//							args[1][1] = "bin";
		//								args[1][2] = NULL;
										//			args[2][0] = "head";
										//			args[2][1] = "-n1";
										//			args[2][2] = NULL;
										//						char *comm = out_delim[0];
										//						comm_parser(&comm, args[0]);
										//							str_cpy(comm, out_delim[1]);
										//						char *comm2 = out_delim[1];
										//						comm_parser(&comm2, args[1]);

																			for(int i=0;i<ctr;i++){
																				char comm[COMM_LEN] = {0};
																				char *temp_comm = comm;
																				str_cpy(temp_comm, out_delim[i]);
																				comm_parser(&temp_comm, args[i]);
										//								
																			}

										//	printf("comm: %s", args[0][0]);

										pipe_comm(args, ctr);
								}
								else{
										if (BACKGROUND > 0){
												args[arg_ctr - 1] = NULL;
												arg_ctr--;
										}
										if (exec_custom(command, args, arg_ctr) == 0){
												if (exec_vp(args[0], args, BACKGROUND) < 0){
														char err[100];
														str_concat("sbush: error command not found: ",args[0], "", err); 
														puts(err);
												}
										}
								}
								((get_env("PS1"))[0] == -1) ? set_env("PS1", PS1_DEFAULT) : set_env("PS1", get_env("PS1"));
								char *PS1 = get_env("PS1");
								int ret = write(1, PS1, sizeof(PS1)-1);
								if (ret < 0)
									puts("sbush: Error writing to STDOUT");

								continue;
						}
						i++;
				}

		} 
		return 0;
}
