#include <sys/defs.h>

void memcpy(void *dest, void* src, int size ){
		char *source = (char *)src;
		char * dst = (char *)dest;
		for(int i = 0; i < size; i ++){
				dst[i] = source[i];
		}
}

void memset (void *address, int value, int size) {
         unsigned char *p = address;
         for (int i = 0; i< size; i++)
             *p++ = (unsigned char)value;
 }
