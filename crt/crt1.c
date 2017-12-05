#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void _start(void) {

	uint64_t argc = 0;
	uint64_t argv1 = 0, argv2 = 0;
__asm__ volatile ("movq %%rax, %0; movq %%rbx, %1; movq %%rcx, %2"
 : "=g" (argc), "=g"(argv1), "=g"(argv2)
 );
 char *argv[1] = {(char*)argv1};
	int ret = main(argc, argv, NULL);
	exit(ret);
  // call main() and exit() here
}
