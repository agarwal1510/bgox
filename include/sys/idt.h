#ifndef _IDT_H
#define _IDT_H

void kmain(void);
void setup_gate(int32_t num, uint64_t handler_addr);

#endif
