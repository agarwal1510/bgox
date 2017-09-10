#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void _start(void) {
/*char hello[10];
int retval;
__asm__ volatile ("int $0x80"
 : "=a" (retval)
  : "a" (3), "b" (1), "c" (hello), "d" (1)
   : "memory");

__asm__ volatile ("int $0x80"
 : "=a" (retval)
  : "a" (4), "b" (1), "c" (hello), "d" (sizeof(hello)-1)
   : "memory");
   __asm__ volatile ("int $0x80" : : "a" (1), "b" (0));
*///	int ret =  
	int ret = main(0, NULL, NULL);
	exit(ret);
  // call main() and exit() here
}
