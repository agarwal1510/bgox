# bgox (BigOx)
A cooperative 64-bit multitasking operating system featuring interrupt driven I/O, x86 compliant 4 level page tables, User and Kernel Level process context switch and scheduling policies, memory virtualization and a read-only file system. When booted, the kernel presents user with a command line interface[oxTerm] for execution of a pre-defined set of command line stack.


*Testing Environment*

```
Hardware Hypervisor: QEMU
The included qem.sh script configures the hypervisor to set up the hardware inside the emulated envornment.

Build instructions:

make
chmod +x ./qem.sh
./qem.sh

* The provided Makefile:
  1) builds a kernel
  2) copies it into rootfs/boot/kernel/kernel
  3) creates an ISO CD image with the rootfs/ contents

* To boot the system in QEMU, run(Not required if building using the provided script):
qemu-system-x86_64 -curses -drive id=boot,format=raw,file=$USER.img,if=none -drive id=data,format=raw,file=$USER-data.img,if=none -device ahci,id=ahci -device ide-drive,drive=boot,bus=ahci.0 -device ide-drive,drive=data,bus=ahci.1 -gdb tcp::9999

Explanation of parameters:
  -curses         use a text console (omit this to use default SDL/VNC console)
  -drive ...      connect a CD-ROM or hard drive with corresponding image
  -device ...     configure an AHCI controller for the boot and data disks
  -gdb tcp::9999  listen for "remote" debugging connections on port NNNN
  -S              wait for GDB to connect at startup
  -no-reboot      prevent reboot when OS crashes

* When using the -curses mode, switch to the qemu> console with ESC-2.
```

------------------------------------

*Supported Binaries*

**NAME:**
echo - displays the text passed as argument

**USAGE:**
echo [STRING]

**NAME:**
sleep - puts the terminal process to sleep for specified amount of time

**USAGE:**
sleep [TimeinSeconds] [&...optional] - Puts the process to sleep in background and clears it up after time expiration

**NAME:**
cat - displays the content of the file (passed as argument)

**USAGE:**
cat [filename]

**NAME:**
ls - displays the entire list of files from the r/o file system

**USAGE:**
ls

**NAME:**
kill -9 - kills the process with pid specified as argument

**USAGE:**
kill -9 [PID]

**NAME:**
ps - reports a snapshot of the processes[running, sleeping] for the system.

**USAGE:**
ps

**NAME:**
clear - Clears the oxTerm sesion screen

**USAGE:**
clear

------------------------------------

### SysCalls definitions
```
STD I/O:

int putchar(int c); 

int puts(const char *s);

void print(int stream, const char *s);

int read(0, char *s, uint16_t size); //STDIN

ssize_t write(int stream, const void *buf, size_t count); //STDOUT, STDERR

int printf(const char *format, ...);

int gets(char *s);
```
---

**Process Management**
```
pid_t fork();

int execvp(char *proc, char *arg);

int exec(char *proc);

pid_t wait(int *status);

pid_t waitpid(int pid);

pid_t getpid(void);

pid_t getppid(void);
```
---

**File system Management**
```
int64_t opendir(char *dir);

uint64_t read_dir(uint64_t dir);

uint64_t closedir(uint64_t dir);

file *open(char *filename);

int close(file *fd);

size_t read_file(file* fd, void *buf, size_t bytes);

size_t readline(file* fd, void *buf, size_t bytes);
```
---

Thanks to OSDEV for their comprehensive refence for kernel development !
