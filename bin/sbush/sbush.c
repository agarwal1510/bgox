#include <stdio.h>
#include <stdlib.h>
//#include <sys/wait.h>
//#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#define WNOHANG 1
#define WUNTRACED 2
#define BIN_LEN  10
#define ARG_LEN  20
#define ENV_LEN 1000
#define PATH_LEN 1000
#define PS1_DEFAULT "user@bgox> "
char ENV_PATH[PATH_LEN] = "bin/:";

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

int exec_vp(char *path, char *args[], int BACKGROUND){
	char paths[ENV_LEN][COMM_LEN];
	int pid;
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
							if (waitpid(pid) == -1){
									continue;
							}
							else{
									ret = 1;
									break;
							}
					}
					else{
							if (waitpid(pid) == -1){
									continue;
							}
							else{
									ret = 1;
									break;
							}
					}
			}
			else{
		//			char *env[] = {ENV_PATH, ENV_PS1, ENV_HOME};
					execvp(args[0],"");
//					execve(args[0], args, env);
			}
			if (i != ctr-1)
				str_cpy(args[0], temp_path);

	}
	return ret;	

}
int main(int argc, char *argv[], char *envp[]) {

//		char command[COMM_LEN] = {'\0'};
	//	char *PS1 = "user@bgox $> ";
	//	print(PS1);
//		int i = 0;
//		read(0, (char *)command, 10);
//		print("After read");
//		print(command);
		pid_t pid = 22;
		
		pid = fork();

		if (pid == 0){
			print(1, "This is child");
		}
		else{
		print(1, "Waiting on child. Giving up");
		yield();
		while(1);
		}

		//		if (command[i] == '\n' || command[i] == 0x0) {
		//				command[i] = 0;
		//				i = 0;
	//					if (str_cmp(command, "\n") > 0)
	//							continue;
	//					char *args[COMM_LEN];
	//					char *comm = command;

	//					int arg_ctr = comm_parser(&comm, args);
	//					args[arg_ctr] = NULL;
	//					int BACKGROUND = -1;
	//					if (exec_vp(args[0], args, BACKGROUND) < 0){
	//							char err[100];
	//							str_concat("sbush: error command not found: ",args[0], "", err); 
	//							print(err);
	//					}
	//					print(PS1);
	//					continue;
	//			}

		exit(1);
}
