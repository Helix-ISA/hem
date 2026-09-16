#ifndef HX_CACHE_H
#define HX_CACHE_H

#include "processor/memory.h"
#include "types.h"

#define L1_SIZE (1024 * 1024)
#define CACHE_LINE_SIZE 64
#define CACHE_SIZE (L1_SIZE / 2)
#define CACHE_LINE_COUNT (CACHE_SIZE / CACHE_LINE_SIZE)

typedef struct {
	b8 valid;
	u32 tag;
	u8 data[CACHE_LINE_SIZE];
} hx_cache_line;

typedef struct {
	hx_cache_line lines[CACHE_LINE_COUNT];

	u64 hits;
	u64 misses;
} hx_cache;

b8 cache_create(hx_cache *cache);
b8 cache_destroy(hx_cache *cache);
b8 cache_flush(hx_cache *cache);

u8  cache_read8(hx_cache *cache, hx_memory *memory, u32 address);
u16 cache_read16(hx_cache *cache, hx_memory *memory, u32 address);
u32 cache_read32(hx_cache *cache, hx_memory *memory, u32 address);
u64 cache_read64(hx_cache *cache, hx_memory *memory, u32 address);

b8 cache_write8(hx_cache *cache, hx_memory *memory, u32 address, u8 value);
b8 cache_write16(hx_cache *cache, hx_memory *memory, u32 address, u16 value);
b8 cache_write32(hx_cache *cache, hx_memory *memory, u32 address, u32 value);
b8 cache_write64(hx_cache *cache, hx_memory *memory, u32 address, u64 value);

#endif
