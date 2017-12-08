#include <string.h>
#include <sys/defs.h>
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
						str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
						prev_ptr = i+1;
				}
		}
		str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
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
char *itoa(uint64_t val, char *str, int32_t base)
{
    *str = '\0'; // Currently pointing to the end of string
    if (val == 0) {
        *--str = '0';
        return str;
    }
    if (base != 10 && base != 16) {
        return str;
    }
    while (val) {
        *--str = "0123456789abcdef"[val%base];
        val = val/base;
    }
    return str;
}
void mem_set (void *address, uint64_t value, uint64_t size) {
		char *p = (char *)address;
		for (uint64_t i = 0; i< size; i++)
				*p++ = (char)value;
}

void *memcpy(void *destination, void *source, uint64_t num) 
{
    uint8_t *dest = (uint8_t *)destination;
    uint8_t *src = (uint8_t *)source;

    while(num--) {
        *dest++ = *src++; 
    }

    return destination;
}
