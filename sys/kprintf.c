#include <sys/kprintf.h>
#include <sys/stdarg.h>

#define FMT_LEN 100
#define X_DEFAULT 0
#define Y_DEFAULT 0
int X = X_DEFAULT;
int Y = Y_DEFAULT;
void print_seq(const char * seq){

		const char *temp1 = seq;
		char* temp2 = (char*)0xb8000 + Y*160 + X;

		for(;*temp1; temp1 += 1) {
			if (*temp1 == '\n'){
		   		Y++;
		   		X = 0;
				temp2 = (char*)0xb8000 + Y*160 + X;
			}
			else{
				temp2 += 2;
				X +=2;
				*temp2 = *temp1;
			}
		}

}
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
int str_cmp(char* str1, char* str2){

		return (*str1 == *str2 && *str1 == '\0') ? 1 : (*str1 == *str2 ? str_cmp(++str1, ++str2) : -1);

}
void str_substr(const char *str, int from, int to, char *out_str){
		int index = 0;
		for(int i = from; i <= to; i++){
				out_str[index] = str[i];
				index++;
		}
		out_str[index] = '\0';
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
int itoa(int num, char *dest){
	int i = 0;
	if (num == 0){
		dest[i++] = '0';
		dest[i++] = 0;
		return i;
	}
	int neg = 0;
	if (num < 0){
		neg = 1;
		num = -num;
	}
//	char rev[FMT_LEN];
	while (num != 0){
		dest[i++] = (num % 10) + '0';
		num /= 10;
	}
	if (neg == 1){
		dest[i++] = '-';
	}
	dest[i++] = '\0';
	char tr[FMT_LEN];
	str_cpy(tr, dest);
	str_reverse(tr, dest);
	return str_len(dest);
}

void kprintf(const char *fmt, ...)
{
		va_list al;
		va_start(al, fmt);

		char fmt_split[10][FMT_LEN];
		int split_ctr = str_split_delim(fmt, '%', fmt_split);
		
		for(int i = 0; i < split_ctr; i++){
				const char* fmt_const = fmt_split[i];
				char identfr[3];
				str_substr(fmt_const, 0, 1, identfr);
				char str[FMT_LEN];
				char trail[FMT_LEN];
				if (str_cmp(identfr, "%d") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;

					const int num = va_arg(al, int);
					char temp_tr[FMT_LEN];
					itoa(num, temp_tr);
					
					str_concat(temp_tr, const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str);
				}
				else if (str_cmp(identfr, "%s") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					str_concat(va_arg(al, char*), const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str);
				}
				else if (str_cmp(identfr, "%p") > 0){
					print_seq("pointer found\n");
				}
				else if (str_cmp(identfr, "%x") > 0){
					print_seq("x found");
				}
				else{print_seq(fmt_const);}
		//		print_seq(fmt_split[i]);
		//		print_seq("\n");
		}
while(1);
}
