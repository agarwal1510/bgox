#ifndef _UTILS_H
#define _UTILS_H

uint64_t pow(uint64_t x, int exp);
uint64_t max(uint64_t a, uint64_t b);
int atoi(char *str);
uint64_t octTodec(uint64_t octal);
uint64_t get_starting_page(uint64_t addr);
uint64_t get_ending_page(uint64_t addr);

void memcpy(void *dest, void* src, int size );
void memset (void *address, int value, int size);
char *itoa(uint64_t val, char *str, int32_t base);

#endif
