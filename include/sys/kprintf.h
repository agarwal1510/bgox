#ifndef __KPRINTF_H
#define __KPRINTF_H
extern int X;
extern int Y;
void kprintf(const char *fmt, ...);
void kprintf_at(const char *fmt, ...);
void kprintf_boott(const char *seq, int boott);
void clear_screen();
void move_csr();
#endif
