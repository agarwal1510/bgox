#include <sys/defs.h>

uint64_t pow(uint64_t x, int exp){
	
	if (exp == 0)
		return 1;
	return x*pow(x, exp-1);
}
uint64_t max(uint64_t a, uint64_t b){

	return (a > b) ? a : b;
}
int atoi(char *str){

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
