#ifndef HX_PROCESSOR_H
#define HX_PROCESSOR_H

#include "cli/cl_parser.h"
#include "processor/cache.h"
#include "processor/memory.h"
#include "types.h"

#define GPR_COUNT 32

typedef struct {
	u64 pc;
	u64 gpr[GPR_COUNT];

	b8 halted;

	hx_cache l1i;
	hx_cache l1d;
} hx_processor;

b8 processor_create(hx_processor *processor);
b8 processor_destroy(hx_processor *processor);

b8 processor_reboot(hx_processor *processor);
b8 processor_run(hx_processor *processor, hx_memory *memory, hx_cli *cli);

#endif
