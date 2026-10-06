#include "processor/memory.h"

#include "types.h"

#include <stdio.h>
#include <stdlib.h>

b8 memory_create(hx_memory *memory)
{
	memory->memory = malloc(MAX_MEMORY);

	if (memory->memory == NULL) {
		fprintf(stderr, "out of memory\n");
		return failure;
	}

	return success;
}

b8 memory_destroy(hx_memory *memory)
{
	free(memory->memory);

	return success;
}

u8  memory_read8(hx_memory *memory, u32 address)
{
	return memory->memory[address];
}

u16 memory_read16(hx_memory *memory, u32 address)
{
	return ((u16)memory->memory[address])       |
		((u16)memory->memory[address + 1] <<  8);
}

u32 memory_read32(hx_memory *memory, u32 address)
{
	return ((u32)memory->memory[address])        |
		((u32)memory->memory[address + 1] <<  8) |
		((u32)memory->memory[address + 2] << 16) |
		((u32)memory->memory[address + 3] << 24);
}

u64 memory_read64(hx_memory *memory, u32 address)
{
	return ((u64)memory->memory[address])        |
		((u64)memory->memory[address + 1] <<  8) |
		((u64)memory->memory[address + 2] << 16) |
		((u64)memory->memory[address + 3] << 24) |
		((u64)memory->memory[address + 4] << 32) |
		((u64)memory->memory[address + 5] << 40) |
		((u64)memory->memory[address + 6] << 48) |
		((u64)memory->memory[address + 7] << 56);
}

b8 memory_write8(hx_memory *memory, u32 address, u8 value)
{
	memory->memory[address] = value;

	return success;
}

b8 memory_write16(hx_memory *memory, u32 address, u16 value)
{
	memory->memory[address] 	= (value)      & 0xFF;
	memory->memory[address + 1] = (value >> 8) & 0xFF;

	return success;
}

b8 memory_write32(hx_memory *memory, u32 address, u32 value)
{
	memory->memory[address] 	= (value)       & 0xFF;
	memory->memory[address + 1] = (value >> 8)  & 0xFF;
	memory->memory[address + 2] = (value >> 16) & 0xFF;
	memory->memory[address + 3] = (value >> 24) & 0xFF;

	return success;
}

b8 memory_write64(hx_memory *memory, u32 address, u64 value)
{
	memory->memory[address] 	= (value)       & 0xFF;
	memory->memory[address + 1] = (value >> 8)  & 0xFF;
	memory->memory[address + 2] = (value >> 16) & 0xFF;
	memory->memory[address + 3] = (value >> 24) & 0xFF;
	memory->memory[address + 4] = (value >> 32) & 0xFF;
	memory->memory[address + 5] = (value >> 40) & 0xFF;
	memory->memory[address + 6] = (value >> 48) & 0xFF;
	memory->memory[address + 7] = (value >> 56) & 0xFF;

	return success;
}
