#ifndef HX_PROCESSOR_H
#define HX_PROCESSOR_H

#include "processor/memory.h"
#include "types.h"

#include <stdio.h>

#define GPR_COUNT 32

typedef struct {
	u64 pc;
	u64 gpr[GPR_COUNT];
} hx_processor;

b8 processor_create(hx_processor *processor);
b8 processor_destroy(hx_processor *processor);

b8 processor_reboot(hx_processor *processor);
b8 processor_run(hx_processor *processor, hx_memory *memory, FILE *in);

#endif
