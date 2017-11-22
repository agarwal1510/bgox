#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/strings.h>

tarfs_e tarfs_list[100];

uint64_t opendir(char *dir) {
	tarfs_e entry;
	int i = 0;
	while (1) {
		entry = tarfs_list[i];
		if (entry.type == TYPE_DIRECTORY && str_cmp(&(entry.name[0]), dir) == 0) {
			return entry.addr;
		}
	}
	kprintf("No such file or directory");
	return 0;
}

/*uint64_t readdir(uint64_t addr) {
	tarfs_e entry;
	char *dir;
	int i = 0;

	while (true) {
		
	}
}*/


