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
