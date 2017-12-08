#include <sys/syscall.h>
#include <sys/defs.h>


DEFN_SYSCALL3(read_file, 38, file *, void *, size_t);

size_t read_file(file *fd, void *buf, size_t size)
{
        int bytes = syscall_read_file(fd, buf, size);
        return bytes;
}
