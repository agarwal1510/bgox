#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

int main(int argc, char *argv[], char *envp[]);
void exit(uint64_t status);

void *malloc(size_t size);
//void free(void *ptr);
#endif
