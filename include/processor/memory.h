#ifndef HX_MEMORY_H
#define HX_MEMORY_H

#include "types.h"

#define MAX_MEMORY (1024 * 1024 * 50)

typedef struct {
	u8 *memory;
} hx_memory;

b8 memory_create(hx_memory *memory);
b8 memory_destroy(hx_memory *memory);

u8  memory_read8(hx_memory *memory, u32 address);
u16 memory_read16(hx_memory *memory, u32 address);
u32 memory_read32(hx_memory *memory, u32 address);
u64 memory_read64(hx_memory *memory, u32 address);

b8 memory_write8(hx_memory *memory, u32 address, u8 value);
b8 memory_write16(hx_memory *memory, u32 address, u16 value);
b8 memory_write32(hx_memory *memory, u32 address, u32 value);
b8 memory_write64(hx_memory *memory, u32 address, u64 value);

#endif
