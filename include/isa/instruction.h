#ifndef HX_INSTRUCTION_H
#define HX_INSTRUCTION_H

#include "types.h"
#include "isa/mnemonic.h"
#include "isa/register.h"
#include "isa/operand.h"

typedef struct {
	hx_mnemonic mnemonic;
	hx_width width;

	hx_operand operands[MAX_OPERANDS];
	u32 operand_count;

	u32 line;
} hx_instruction;

hx_instruction instruction_decode(u32 encoded);

#endif
