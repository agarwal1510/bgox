#ifndef __KPRINTF_H
#define __KPRINTF_H

void kprintf(const char *fmt, ...);
void kprintf_at(const char *fmt, ...);
void kprintf_boott(const char *seq, int boott);
void clear_screen();
#endif
