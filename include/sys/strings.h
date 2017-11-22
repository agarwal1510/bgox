#ifndef _STRINGS_H
#define _STRINGS_H

#include <sys/defs.h>

#define FMT_LEN 100

int str_cmp(const char *s1, const char *s2);
int strn_cmp(const char *s, const char *d, size_t n);
int str_len(const char *s);
void str_cpy(char *to_str, char *from_str);
void str_concat(const char *prev, const char *current, char *dest);
int str_split_delim(const char *str, char delim, char out[][FMT_LEN]);
void str_reverse(char *str, char *dest);
void str_substr(const char *str, int from, int to, char *out_str);

#endif
