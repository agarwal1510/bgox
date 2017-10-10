#include <sys/defs.h>

void apicwrite(uint64_t reg, uint64_t value)
{
		uint64_t volatile *localapic = (uint64_t volatile *)reg;
		*localapic = value;
}
uint64_t apicread(uint64_t reg)
{
		uint64_t volatile *localapic = (uint64_t volatile *)(reg);
		return *localapic;
}
