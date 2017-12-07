#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utils.h>
#define COMM_LEN 50

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
						str_substr(str, prev_ptr, i-1, out[0]);
						prev_ptr = i+1;
				}
		}
	//	str_substr(str, prev_ptr, i-1, out[1]);
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
		char split_cmd[2][COMM_LEN];
		char cmd[COMM_LEN];
		char pets[10];
		while(1){
				for(int i = 0; i < COMM_LEN; i++){
						input[i] = 0;
						cmd[i] = 0;
				}
				read(0, input, COMM_LEN);
				if (input[0] == '\n')
					continue;
				else if (str_len(input) >= COMM_LEN){
					print("\noxTerm: Careful with command lengths !\n");
					continue;
				}
				str_substr(input, 0, str_len(input)-2, cmd);
				int i, prev_ptr = 0, arg_ctr = 0;
				for(i=0; cmd[i] != '\0'; i++){
						if (cmd[i] == '\t' || cmd[i] == ' '){
								str_substr(cmd, prev_ptr, i-1, split_cmd[arg_ctr++]);
								prev_ptr = i+1;
						}
				}
				str_substr(cmd, prev_ptr, i-1, split_cmd[arg_ctr++]);
				pid_t pid = 22;

						itoa(arg_ctr, pets, 10);
						print(pets);
				pid = fork();

				if (pid == 0){
						

						if (arg_ctr > 1)
							execvp(split_cmd[0], split_cmd[1]);
						else
							exec(split_cmd[0]);
	//					print("here");
						while(1);
				}
				else{
						//		print(pids);
						yield();
						waitpid(0);
//						while(1);
	//					print("below yield");
				}
	//			for(int i = 0; i < COMM_LEN; i++){
	//					input[i] = 0;
	//					cmd[i] = 0;
	//			}
		}
		//	waitpid(0);
		exit(1);
		return 0;
}
