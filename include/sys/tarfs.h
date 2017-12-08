#ifndef _TARFS_H
#define _TARFS_H
#include <sys/defs.h>

#define TARFS_MAX 1024
#define TYPE_DIRECTORY 5
#define TYPE_FILE 0
#define MAX_PATH_LEN 100

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;
typedef struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
}posix_header;

typedef struct tarfs_entry {
  char name[100];
  int size;
  uint64_t addr;
  int type;
}tarfs_e;

typedef struct file_entry {
	char name[MAX_PATH_LEN];
	int size;
	uint64_t addr;
} file;

void init_tarfs();
void list_dir();
uint64_t opendir(char *dir);
uint64_t read_dir(uint64_t dir);
uint64_t closedir(uint64_t dir);

file *open(char *filename);
int close(file *fd);
size_t read_file(file* fd, void *buf, size_t bytes);
#endif
