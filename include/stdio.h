#ifndef _STDIO_H
#define _STDIO_H

#include <sys/defs.h>

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);
void print(const char *s);
int read(int fd, char *s, uint16_t size);
int printf(const char *format, ...);
int gets(char *s);

#endif
