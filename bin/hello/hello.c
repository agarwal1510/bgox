#include<stdio.h>
#include <unistd.h>

int main(){
	print("Teri Maa ki chut");
//	char buf[1024] = {0};
	for(int i = 0; i < 2; i++){
		print("Tfsdfsdfsdfs Maa ki chut");	
	}
	char s[11];
	read(0, s, 10);
	print(s);
	pid_t pid;
	if ((pid = fork()) != -1) 
		print("fork called");
	while(1);
	return 0;
}
