#include <sys/strings.h>
#include <sys/defs.h>


void str_cpy(char *to_str, char *frm_str){
                int i=0;
                for(i=0;frm_str[i] != '\0'; i++){
                                to_str[i] = frm_str[i];
                }
                to_str[i] = '\0';
}

int str_len(const char *str){
                int i=0;
                for(i=0;str[i] != '\0';i++){
                                // spin
                }
                return i;
}

void str_concat(const char *prev, const char *current, char *dest){
                int i=0;
                int j=0;
                for(i = 0;prev[i] != '\0';i++){
                                dest[i] = prev[i];
                }
                dest[i] = '\0';
                for(j = 0;current[j] != '\0';j++){
                                dest[j + i] = current[j];
                }
                dest[i+j] = '\0';
}
int str_cmp(const char* str1, const char* str2){

		return (*str1 == *str2 && *str1 == '\0') ? 1 : (*str1 == *str2 ? str_cmp(++str1, ++str2) : -1);

}
int strn_cmp( const char * s1, const char * s2, size_t n){
		while ( n && *s1 && ( *s1 == *s2 )){
				++s1;
				++s2;
				--n;
		}
		if ( n == 0 )
				return 0;
		else
				return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

int str_split_delim(const char *str, char delim, char out[][FMT_LEN]){
                int prev_ptr = 0;
                int i = 0;
                int arg_ctr = 0;
                for(i=0;str[i] != '\0';i++){
                                if (str[i] == delim){
                                                str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
                                                prev_ptr = i;
                                }
                }
                str_substr(str, prev_ptr, i-1, out[arg_ctr++]);
                return arg_ctr;

}
void str_reverse(char *str, char *dest){
        int len = str_len(str), i=0;
        while(i < len){
                dest[i] = str[len - 1 - i];
                i++;
        }
        dest[i] = '\0';
}

void str_substr(const char *str, int from, int to, char *out_str){
                int index = 0;
                for(int i = from; i <= to; i++){
                                out_str[index] = str[i];
                                index++;
                }
                out_str[index] = '\0';
}

int str_contains(char *str, char *query){
                  int j=0, i=0;
                  int startIdx = -1, found = 0;
                  for(i=0;str[i] != '\0';i++){
                                  if (str[i] == query[j]){
  //                                              print("found");
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
  //                              print("i ");
                  }
 
                  return (found == 1) ?  startIdx - str_len(query) + 1: -1; // Start Index of query substring
  }
