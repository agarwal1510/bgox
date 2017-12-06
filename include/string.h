#ifndef _STRING_H
#define _STRING_H

#define COMM_LEN 50

void str_cpy(char *to_str, char *frm_str);
int str_len(char *str);
int str_cmp(char* str1, char* str2);
int strfind_occurence(char *str, char query, int occr);
int str_contains(char *str, char *query);
int strfind_delim(char *str, int frm);
void str_substr(char *str, int from, int to, char *out_str);
int split_delim(char *str, char delim, char out[][COMM_LEN]);
int split(char *str, char out[][COMM_LEN]);
void str_concat(char *prev, char *current, char *after, char *dest);


#endif
