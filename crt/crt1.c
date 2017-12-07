#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void _start(void) {

	     uint64_t argc, argv1, argv2;

	     __asm__ volatile("movq %%rax, %0;"
	             "movq %%rbx, %1;"
	             "movq %%rcx, %2;"
	             : "=g"(argc),"=g"(argv1), "=g"(argv2)
	             :
	             :"rax", "rcx", "rbx", "rsi"
	               );
	int ret;
	if (argc == 0){
		print(1, "zero");
		ret = main(argc, NULL,  NULL);	
	}
	else{
 		char *argv[1] = {(char*)argv1};
		ret = main(argc, argv,  NULL);
	}
	exit(ret);
  // call main() and exit() here
}
