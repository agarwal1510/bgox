#ifndef _PORTIO_H
#define _PORTIO_H

void outb(int16_t port, uint8_t value);
uint8_t inb(int16_t port);
void outl(int16_t port, uint32_t value);
long inl(int16_t port);

#endif
