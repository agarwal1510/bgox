#include <sys/kprintf.h>
#include <sys/stdarg.h>
#include <sys/defs.h>
#include <sys/strings.h>
#include <sys/paging.h>
#define X_DEFAULT 0
#define Y_DEFAULT 0
int X = X_DEFAULT;
int Y = Y_DEFAULT;
uint64_t vd_addr = VADDR(0xb8000);
void str_cpy(char *to_str, char *frm_str);
void scroll_up(){
			for(int i = 1; i < 24; i++){
				char *next = (char*)vd_addr + i*160;
					for(int j = 0; j < 160; j++){
						char *top = (char*)vd_addr + (i-1)*160;
						*(top + j) = *(next + j);
						str_cpy((next+j),"");
					}
			}
			Y = 23;
			X = 0;
}
void print_seq(const char * seq, int x, int y){

		int overwrite_cood = 0;
		if (x == -1){
			x = X;
			overwrite_cood = 1;
		}
		if (y == -1){
			y = Y;
			overwrite_cood = 1;
		}
		const char *temp1 = seq;
	//	if (flag == 1)
	//		while(1);

		char* temp2 = (char*)vd_addr + y*160 + x;
		for(;*temp1; temp1 += 1) {
			if (*temp1 == '\n'){
		   		y++;
		   		x = 0;
				if (y > 23){
					scroll_up();
					y = 23;
				}
				temp2 = (char*)vd_addr + y*160 + x;
			}
			else if (*temp1 == '\r'){
		   		y = 0;
				temp2 = (char*)vd_addr + y*160 + x;
			}
			else{
				x +=2;
				if (x >= 160){
					y++;
					x = 0;
					if (y > 23){
						scroll_up();
						y = 23;
					}
				}
				temp2 = (char*)vd_addr + y*160 + x;
				*temp2 = *temp1;
			}
		}
		if (overwrite_cood == 1){
			X = x;
			Y = y;
		}

}

int itoa(unsigned long long num, char *dest, int base){
	unsigned long long i = 0;
	if (num == 0){
		dest[i++] = '0';
		dest[i++] = 0;
		return i;
	}
	int neg = 0;
	if (num < 0 && base == 10){
		neg = 1;
		num = -num;
	}
	while (num != 0){
		if (base == 10){
		dest[i++] = (num % 10) + '0';
		num /= 10;
		}
		else if (base == 16){
			unsigned long long rem = num % 16;
			if (rem > 9){
				dest[i++] = rem - 10 + 'a';
			}
			else
				dest[i++] = rem + '0';		
		num /= 16;
		}
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

void kprintf_boott(const char *seq, int sec){
// For static printing, No New lines, No Fancy stuff.
	int y = 24;
	int x = 100;
	char time_str[FMT_LEN];
	char boots[FMT_LEN];
	itoa(sec, boots, 10);
	str_concat(seq, boots, time_str);
	const char *temp1 = time_str;
	char* temp2 = (char*)vd_addr + y*160 + x;	
	for(;*temp1; temp1 += 1) {
				temp2 += 2;
				x +=2;
				*temp2 = *temp1;
	}
}
void print_wrapper(const char *fmt, ...){

		va_list al;
		va_start(al, fmt);

}
void kprintf_at(const char *fmt, ...)
{
		va_list al;
		va_start(al, fmt);
		int x = va_arg(al, int);
		int y = va_arg(al, int);

		char fmt_split[10][FMT_LEN];
		int split_ctr = str_split_delim(fmt, '%', fmt_split);
		for(int i = 0; i < split_ctr; i++){
				const char* fmt_const = fmt_split[i];
				char identfr[3];
				str_substr(fmt_const, 0, 1, identfr);
				char str[FMT_LEN];
				char trail[FMT_LEN];
				if (str_cmp(identfr, "%d") > 0 || str_cmp(identfr, "%x") > 0 || str_cmp(identfr, "%p") > 0 ){
					int base = 10;
					if (str_cmp(identfr, "%p") > 0 || str_cmp(identfr, "%x") > 0)
						base = 16;
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					const unsigned long long num = va_arg(al, unsigned long long);

					char temp_tr[FMT_LEN];
					itoa(num, temp_tr, base);
					
					str_concat(temp_tr, const_trail, str);
					char final_str[FMT_LEN];
					str_cpy(final_str, str);
					if(str_cmp(identfr, "%p") > 0){
						char *max_Addr = "0x";
					//	char pref[64];
					//	str_substr(max_Addr, 0, 13 - str_len(final_str), pref);
						str_concat(max_Addr, str, final_str);
					}
					const char* const_str = final_str;
					print_seq(const_str, x, y);
				}
				else if (str_cmp(identfr, "%s") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					str_concat(va_arg(al, char*), const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str, x, y);
				}
				else if (str_cmp(identfr, "%c") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					char ch = (char)va_arg(al, int);
					str_concat(&ch, const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str, x, y);
				}
				else{
				print_seq(fmt_const, x, y);}
		}
		va_end(al);
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
				if (str_cmp(identfr, "%d") > 0 || str_cmp(identfr, "%x") > 0 || str_cmp(identfr, "%p") > 0 ){
					int base = 10;
					if (str_cmp(identfr, "%p") > 0 || str_cmp(identfr, "%x") > 0)
						base = 16;
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;

					const unsigned long long num = va_arg(al, uint64_t);
					char temp_tr[FMT_LEN];
					itoa(num, temp_tr, base);
					
					str_concat(temp_tr, const_trail, str);
					char final_str[FMT_LEN];
					str_cpy(final_str, str);
					if(str_cmp(identfr, "%p") > 0){
						char *max_Addr = "0x";
					//	char pref[20];
					//	str_substr(max_Addr, 0, 13 - str_len(final_str), pref);
						str_concat(max_Addr, str, final_str);
					}
					const char* const_str = final_str;
					print_seq(const_str, -1, -1);
				}
				else if (str_cmp(identfr, "%s") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					str_concat(va_arg(al, char*), const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str, -1, -1);
				}
				else if (str_cmp(identfr, "%c") > 0){
					str_substr(fmt_const, 2, str_len(fmt_const)-1, trail);
					const char* const_trail = trail;
					char ch = (char)va_arg(al, int);
					str_concat(&ch, const_trail, str) ;
					const char* const_str = str;
					print_seq(const_str, -1, -1);
				}
				else{print_seq(fmt_const, -1, -1);}
		}
		va_end(al);
}
