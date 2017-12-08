#include <sys/defs.h>

uint64_t pow(uint64_t x, int exp){
	
	if (exp == 0)
		return 1;
	return x*pow(x, exp-1);
}
uint64_t max(uint64_t a, uint64_t b){

	return (a > b) ? a : b;
}
uint64_t atoi(char *str){

	int res = 0, sign = 1, i = 0;
	
	if (str[0] == '-'){
		sign = -1;
		i++;
	}
	
	while(str[i] != '\0'){
		res = res*10 + str[i] - '0';
		i++;
	}
	return sign*res;
}
uint64_t octTodec(uint64_t octal){

        uint64_t decimal = 0, i=0;
        while(octal != 0){
                decimal += (octal % 10)*pow(8, i++);
                octal = octal / 10;
        }   
        return decimal;
}

uint64_t get_starting_page(uint64_t addr) {
	return addr - addr % 0x1000;
}

uint64_t get_ending_page(uint64_t addr) {
	return get_starting_page(addr + (uint64_t)0x1000 - 1);
}

void memcpy(void *dest, void* src, uint64_t size ){
		char *source = (char *)src;
		char * dst = (char *)dest;
		for(uint64_t i = 0; i < size; i ++){
				dst[i] = source[i];
		}
}

void memset (void *address, uint64_t value, uint64_t size) {
         char *p = (char *)address;
         for (uint64_t i = 0; i< size; i++)
             *p++ = (char)value;
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
