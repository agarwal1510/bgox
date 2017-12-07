#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/strings.h>
#include <sys/mem.h>
#include <sys/utils.h>

tarfs_e tarfs_list[TARFS_MAX];
int tarf_idx = 0;
int bytesdone = 0;
static int skip_size = sizeof(posix_header);

void init_tarfs(){
	
	posix_header *tarfs_head = (posix_header *)&_binary_tarfs_start;
	int temp_ptr = 0;
	int idx = 0;
	

	while(str_len(tarfs_head->name) > 0){
//		kprintf("%s %s\n", tarfs_head->name, tarfs_head->size);
		
		tarfs_e tarfs_entry;
		
		int entry_size = octTodec(atoi(tarfs_head->size));
		
		str_cpy(tarfs_entry.name, tarfs_head->name);
		tarfs_entry.size = entry_size;
		tarfs_entry.type = atoi(tarfs_head->typeflag);
		tarfs_entry.addr = (uint64_t)tarfs_head;
		tarfs_list[idx++] = tarfs_entry;

		if (entry_size == 0)
			temp_ptr += skip_size;
		else{
			if (entry_size % skip_size == 0){
				temp_ptr += entry_size + skip_size;
			}
			else
				temp_ptr += entry_size + (skip_size - entry_size % skip_size) + skip_size;
		}
		tarfs_head = (posix_header *)(&_binary_tarfs_start + temp_ptr);	
	}
	
	tarf_idx = idx;

}

uint64_t opendir(char *dir) {
		tarfs_e entry;
		for(int i = 0; i < tarf_idx; i++){
				entry = tarfs_list[i];
				kprintf("%s %s %d\n", entry.name, dir, entry.type);
				if (entry.type == TYPE_DIRECTORY && (str_cmp(entry.name, dir) == 1)) {
						return (uint64_t)entry.addr;
				}
		}   
		kprintf("Opendir %s: No such directory\n", dir);
		return 0;
}
uint64_t closedir(uint64_t addr){
	return 1;
}

void list_dir() {
		tarfs_e entry;
		for(int i = 0; i < tarf_idx; i++){
				entry = tarfs_list[i];
				if (entry.type == TYPE_DIRECTORY) {
						kprintf("%s    		DIR\n", entry.name);
				}
				else if (entry.type == TYPE_FILE){
						kprintf("%s    		FILE\n", entry.name);
				}
		}   
//		kprintf("Opendir %s: No such directory\n", dir);
//		return 0;
}

uint64_t read_dir(uint64_t addr)
{
		tarfs_e entry;
		char dir[MAX_PATH_LEN];
		int parent = -1;
		for(int i = 0; i < tarf_idx && str_len(entry.name) > 0; i++){
				entry = tarfs_list[i];
				if(entry.addr == addr){
					parent = i;
					str_cpy(dir, entry.name);
	//				kprintf("Found parent: %s %d\n",dir,len); 
				}  
				else if(parent >= 0 && strn_cmp(entry.name, dir, str_len(dir)) == 0){  
					
					kprintf("Reading contents: %s in %s\n", entry.name, dir);
				}  
		}  
		return 0;
}


file *open(char *filename) {
		file* fd = (file *) kmalloc(1);
		tarfs_e entry;
		for(int i = 0; i < tarf_idx; i++){
				entry = tarfs_list[i];
				if (str_cmp(filename, entry.name) == 1 && entry.type == TYPE_FILE) {
						fd->addr = entry.addr;
		//				kprintf("size: %d, entry.size", entry.size);
						fd->size = entry.size;
						str_cpy(fd->name, filename);
						return fd;
				}
				else if (str_cmp(filename, entry.name) == 1 && entry.type == TYPE_DIRECTORY) {
//						kprintf("%s: is a directory.", filename);
						return NULL;
				}
		}
//		kprintf("%s: No such file\n", filename);
		return NULL;
}        

int close(file *fd) {
		fd->addr = 0;
		fd->size = 0;
		str_cpy(fd->name, "");
		bytesdone = 0;
//		free((uint64_t *)fd);
		return 1;
}


size_t read(file* fd, void *buf, size_t bytes){
	if (fd->size == 0)
		return 0;
	int bytestoRead = (max(bytes, fd->size - bytesdone) == fd->size - bytesdone ? bytes : fd->size - bytesdone);
	char* fileaddr = (char *)(fd->addr + skip_size + bytesdone);
	int i = 0;
	char *tempbuf = (char *)buf;
	for(i = 0; i < bytestoRead; i++){
		tempbuf[i] = *fileaddr++;
	}
	tempbuf[i] = '\0';
	bytesdone += str_len(tempbuf)-1;
	return bytestoRead;
}

size_t readline(file* fd, void *buf, size_t bytes){
	if (fd->size == 0 || bytesdone - fd->size == 0)
		return 0;
	int bytestoRead = (max(bytes, fd->size - bytesdone) == fd->size - bytesdone ? bytes : fd->size - bytesdone);
	char* fileaddr = (char *)(fd->addr + skip_size + bytesdone);
	int i = 0;
	char *tempbuf = (char *)buf;
	for(i = 0; i < bytestoRead; i++){
		if (*fileaddr == '\n'){
			bytesdone++;
			break;
		}
		tempbuf[i] = *fileaddr++;
	}
	tempbuf[i] = '\0';
//	while(1);
	bytesdone += str_len(tempbuf);
	return str_len(tempbuf);
}
