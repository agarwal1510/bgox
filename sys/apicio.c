#include <sys/defs.h>
#include <sys/kprintf.h>
static inline void apicwrite(uint64_t *reg, uint64_t value)
{
		kprintf("\nWriting: %x %x\n", reg, value);
//		uint64_t volatile *localapic = (uint64_t volatile *)reg;
//		*localapic = value;
		*(volatile uint64_t *)(reg) = value;
}
static inline uint64_t apicread(uint64_t *reg)
{
		kprintf("reading: %x\n", reg);
	//	uint64_t volatile *localapic = (uint64_t volatile *)(reg);
		return *(volatile uint64_t *)(reg);
}
