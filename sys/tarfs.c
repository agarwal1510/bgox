#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/strings.h>

tarfs_e tarfs_list[TARFS_MAX];
int tarf_idx = 0;
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
	while(str_len(tarfs_head->name) > 0){
		kprintf("%s\n", tarfs_head->name);
		
		tarfs_e tarfs_entry;
		
		int entry_size = octTodec(atoi(tarfs_head->size));
		
		str_cpy(tarfs_entry.name, tarfs_head->name);
		tarfs_entry.size = entry_size;
		tarfs_entry.type = atoi(tarfs_head->typeflag);
		tarfs_entry.addr = (uint64_t)tarfs_head;
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
	
	tarf_idx = idx;

}

uint64_t opendir(char *dir) {
		tarfs_e entry;
		for(int i = 0; i < tarf_idx; i++){
				entry = tarfs_list[i];
//				kprintf("query: %s %s %d\n", entry.name, dir, entry.type);
				if (entry.type == TYPE_DIRECTORY && (str_cmp(entry.name, dir) == 1)) {
						return (uint64_t)entry.addr;
				}
		}   
		kprintf("No such file or directory");
		return 0;
}

uint64_t read_dir(uint64_t addr)
{
		tarfs_e entry;
		char dir[MAX_PATH_LEN];
		int len = 0, parent = -1;
		for(int i = 0; i < tarf_idx && str_len(entry.name) > 0; i++){
				entry = tarfs_list[i];
				if(entry.addr == addr){
					parent = i;
					str_cpy(dir, entry.name);
					len = str_len(dir);
	//				kprintf("Found parent: %s %d\n",dir,len); 
				}  
				else if(parent >= 0 && strn_cmp(entry.name, dir, str_len(dir)) == 0){  
					
					kprintf("Reading: %s %s %d %s\n", entry.name, dir, str_len(dir), entry.name + len);
				}  
		}  
		return 0;
}
