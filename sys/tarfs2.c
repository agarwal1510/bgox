#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/strings.h>
#include <sys/mem.h>

tarfs_e tarfs_list[100];

uint64_t opendir(char *dir) {
	tarfs_e entry;
	int i = 0;
	while (1) {
		entry = tarfs_list[i];
		if (entry.type == TYPE_DIRECTORY && str_cmp(&(entry.name[0]), dir) == 0) {
			return entry.addr;
		}
		i++;
	}
	kprintf("No such file or directory");
	return 0;
}

/*uint64_t readdir(uint64_t addr) {
	tarfs_e entry;
	char *dir;
	int i = 0;

	while (true) {
		entry = tarfs_list[i];
		if (entry.addr == addr) {
				
		}
	}
}*/

file *open(char *filename) {
	file* fd = (file *) kmalloc(sizeof(file));
	int i = 0;
	tarfs_e entry;
	while (1) {
		entry = tarfs_list[i];
		if (str_cmp(filename, &(entry.name[0])) == 0 && entry.type == TYPE_FILE) {
			fd->addr = entry.addr;
			str_cpy(fd->filename, filename);
			return fd;
		}

		i++;
	}
	kprintf("No such file or directory");
	return NULL;
}

int close(file *fd) {
	fd->addr = 0;
	str_cpy(fd->filename, "");
	return 1;
}

int closedir(uint64_t addr) {
	return 1;
}

