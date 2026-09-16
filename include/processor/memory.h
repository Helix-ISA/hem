#ifndef HX_MEMORY_H
#define HX_MEMORY_H

#include "types.h"

#define MAX_MEMORY (1024 * 1024 * 50)

typedef struct {
	u8 *memory;
} hx_memory;

b8 memory_create(hx_memory *memory);
b8 memory_destroy(hx_memory *memory);

#endif
