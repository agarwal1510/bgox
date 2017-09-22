#include <sys/defs.h>

void outb(int16_t port, uint8_t value){
	__asm__ volatile("outb %0, %1"
					:
					: "a" (value), "Nd" (port));
}

uint8_t inb(int16_t port){
	uint8_t ret;
	__asm__ volatile("inb %1, %0"
					: "=a" (ret)
					: "Nd" (port));
	return ret;
}

void outl(int16_t port, uint32_t value){
	__asm__ volatile("outl %0, %1"
					:
					: "a" (value), "Nd" (port));
}

long inl(int16_t port){
	uint32_t ret;
	__asm__ volatile("inl %1, %0"
					: "=a" (ret)
					: "Nd" (port));
	return (long)ret;
}
