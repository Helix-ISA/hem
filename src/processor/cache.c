#include "processor/cache.h"

#include "processor/memory.h"
#include "types.h"

#include <string.h>

static u32 cache_get_index(u32 address)
{
	return (address / CACHE_LINE_SIZE) % CACHE_LINE_COUNT;
}

static u32 cache_get_tag(u32 address)
{
	return address / (CACHE_LINE_SIZE * CACHE_LINE_COUNT);
}

static u32 cache_get_offset(u32 address)
{
	return address % CACHE_LINE_SIZE;
}

static u32 cache_get_line_address(u32 address)
{
	return address - cache_get_offset(address);
}

static b8 cache_address_valid(u32 address, u32 size)
{
	if (address >= MAX_MEMORY)
		return failure;

	if (size > MAX_MEMORY - address)
		return failure;

	return success;
}

static b8 cache_fill(hx_cache *cache, hx_memory *memory, u32 address)
{
	u32 index = cache_get_index(address);
	u32 tag = cache_get_tag(address);
	u32 line_address = cache_get_line_address(address);

	hx_cache_line *line = &cache->lines[index];

	if (line_address > MAX_MEMORY - CACHE_LINE_SIZE)
		return failure;

	memcpy(
		line->data,
		&memory->memory[line_address],
		CACHE_LINE_SIZE
	      );
	line->valid = true;
	line->tag = tag;

	return success;
}

b8 cache_create(hx_cache *cache)
{
	if (cache == NULL)
		return failure;

	return cache_flush(cache);
}

b8 cache_destroy(hx_cache *cache)
{
	if (cache == NULL)
		return failure;

	return cache_flush(cache);
}

b8 cache_flush(hx_cache *cache)
{
	if (cache == NULL)
		return failure;

	memset(cache->lines, 0, sizeof(cache->lines));

	cache->hits = 0;
	cache->misses = 0;

	return success;
}

u8 cache_read8(hx_cache *cache, hx_memory *memory, u32 address)
{
	u32 index;
	u32 tag;
	u32 offset;

	hx_cache_line *line;

	if (cache == NULL || memory == NULL)
		return 0;

	if (!cache_address_valid(address, sizeof(u8)))
		return 0;

	index = cache_get_index(address);
	tag = cache_get_tag(address);
	offset = cache_get_offset(address);

	line = &cache->lines[index];

	if (line->valid && line->tag == tag) {
		cache->hits++;
		return line->data[offset];
	}

	cache->misses++;

	if (!cache_fill(cache, memory, address))
		return 0;

	return cache->lines[index].data[offset];
}

u16 cache_read16(hx_cache *cache, hx_memory *memory, u32 address)
{
	u16 value = 0;
	value |= (u16)cache_read8(cache, memory, address);
	value |= (u16)cache_read8(cache, memory, address + 1) << 8;

	return value;
}

u32 cache_read32(hx_cache *cache, hx_memory *memory, u32 address)
{
	u32 value = 0;
	value |= (u32)cache_read8(cache, memory, address);
	value |= (u32)cache_read8(cache, memory, address + 1) << 8;
	value |= (u32)cache_read8(cache, memory, address + 2) << 16;
	value |= (u32)cache_read8(cache, memory, address + 3) << 24;

	return value;
}

u64 cache_read64(hx_cache *cache, hx_memory *memory, u32 address)
{
	u64 value = 0;
	value |= (u64)cache_read8(cache, memory, address);
	value |= (u64)cache_read8(cache, memory, address + 1) << 8;
	value |= (u64)cache_read8(cache, memory, address + 2) << 16;
	value |= (u64)cache_read8(cache, memory, address + 3) << 24;
	value |= (u64)cache_read8(cache, memory, address + 4) << 32;
	value |= (u64)cache_read8(cache, memory, address + 5) << 40;
	value |= (u64)cache_read8(cache, memory, address + 6) << 48;
	value |= (u64)cache_read8(cache, memory, address + 7) << 56;

	return value;
}

b8 cache_write8(hx_cache *cache, hx_memory *memory, u32 address, u8 value)
{
	u32 index;
	u32 tag;
	u32 offset;

	hx_cache_line *line;

	if (cache == NULL || memory == NULL)
		return failure;

	if (!cache_address_valid(address, sizeof(u8)))
		return failure;

	index = cache_get_index(address);
	tag = cache_get_tag(address);
	offset = cache_get_offset(address);

	line = &cache->lines[index];

	memory->memory[address] = value;

	if (line->valid && line->tag == tag) {
		line->data[offset] = value;
	}

	return success;
}

b8 cache_write16(hx_cache *cache, hx_memory *memory, u32 address, u16 value)
{
	if (!cache_address_valid(address, sizeof(u16)))
		return failure;

	if (!cache_write8(cache, memory, address,
			(u8)(value & 0xff)))
		return failure;

	if (!cache_write8(cache, memory, address + 1,
			(u8)((value >> 8) & 0xff)))
		return failure;

	return success;
}

b8 cache_write32(hx_cache *cache, hx_memory *memory, u32 address, u32 value)
{
	if (!cache_address_valid(address, sizeof(u32)))
		return failure;

	if (!cache_write8(cache, memory, address,
			(u8)(value & 0xff)))
		return failure;

	if (!cache_write8(cache, memory, address + 1,
			(u8)((value >> 8) & 0xff)))
		return failure;

	if (!cache_write8(cache, memory, address + 2,
			(u8)((value >> 16) & 0xff)))
		return failure;

	if (!cache_write8(cache, memory, address + 3,
			(u8)((value >> 24) & 0xff)))
		return failure;

	return success;
}

b8 cache_write64(hx_cache *cache, hx_memory *memory, u32 address, u64 value)
{
	if (!cache_address_valid(address, sizeof(u64)))
		return failure;

	for (u32 i = 0; i < sizeof(u64); i++) {
		if (!cache_write8(
			cache,
			memory,
			address + i,
			(u8)((value >> (i * 8)) & 0xff)
		)) {
			return failure;
		}
	}

	return success;
}
