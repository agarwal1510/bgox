#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
tarfs_e tarfs_list[TARFS_MAX];

int strlen(const char *str){
	int len = 0;
    for (len = 0; str[len]; (len)++);

	return len;
}

void strcpy(char *to_str, char *frm_str){
		int i=0;
		for(i=0;frm_str[i] != '\0'; i++){
				to_str[i] = frm_str[i];
		}
		to_str[i] = '\0';
}

uint64_t power (uint64_t x, int e) {
		if (e == 0) return 1;

        return x * power(x, e-1);
}

int atoi(char *str)
{
        int res = 0;  // Initialize result
        int sign = 1;  // Initialize sign as positive
        int i = 0;  // Initialize index of first digit

        // If number is negative, then update sign
        if (str[0] == '-')
        {   
                sign = -1;  
                i++;  // Also update index of first digit
        }   

        // Iterate through all digits and update the result
        for (; str[i] != '\0'; ++i)
                res = res*10 + str[i] - '0';

        // Return result with sign
        return sign*res;
}
uint64_t octTodec(uint64_t octal)
{
        uint64_t decimal = 0, i=0;
        while(octal!=0){
                decimal = decimal + (octal % 10) * power(8,i++);
                octal = octal/10;
        }   
        return decimal;
}
void init_tarfs(){
	
	posix_header *tarfs_head = (posix_header *)&_binary_tarfs_start;
	int temp_ptr = 0;
	int idx = 0;
	while(strlen(tarfs_head->name) != 0){
		kprintf("%s\n", tarfs_head->name);
		
		tarfs_e tarfs_entry;
		
		int entry_size = octTodec(atoi(tarfs_head->size));
		
		strcpy(tarfs_entry.name, tarfs_head->name);
		tarfs_entry.size = entry_size;
		tarfs_list[idx++] = tarfs_entry;
		
		if (entry_size == 0)
			temp_ptr += 512;
		else{
			if (entry_size % 512 == 0){
				temp_ptr += entry_size + 512;
			}
			else
				temp_ptr += entry_size + (512 - entry_size % 512) + 512;
		}
		tarfs_head = (posix_header *)(&_binary_tarfs_start + temp_ptr);
		
	}

}
