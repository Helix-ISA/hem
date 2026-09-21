#include "processor/processor.h"

#include "isa/instruction.h"
#include "isa/mnemonic.h"
#include "processor/cache.h"
#include "processor/memory.h"
#include "types.h"
#include <stdio.h>

b8 processor_create(hx_processor *processor)
{
	if (processor == NULL)
		return failure;

	if (!cache_create(&processor->l1i))
		return failure;

	if (!cache_create(&processor->l1d)) {
		cache_destroy(&processor->l1i);
		return failure;
	}

	return processor_reboot(processor);
}

b8 processor_destroy(hx_processor *processor)
{
	if (processor == NULL)
		return failure;

	cache_destroy(&processor->l1i);
	cache_destroy(&processor->l1d);

	return success;
}

b8 processor_reboot(hx_processor *processor)
{
	if (processor == NULL)
		return failure;

	processor->pc = 0x0;

	for (u8 i = 0; i < GPR_COUNT; i++) {
		processor->gpr[i] = 0;
	}

	cache_flush(&processor->l1i);
	cache_flush(&processor->l1d);

	return success;
}

static u32 processor_fetch32(hx_processor *processor, hx_memory *memory)
{
	u32 instruction;

	instruction = cache_read32(&processor->l1i, memory, processor->pc);

	processor->pc += 4;

	return instruction;
}

static b8 processor_execute(hx_processor *processor, hx_memory *memory, hx_instruction *instruction)
{
	switch (instruction->mnemonic) {
		case HX_MN_ADD: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] + processor->gpr[rs2];

			return success;
		}
		case HX_MN_ADDI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			s64 imm = (s64)(instruction->operands[2].value.imm & 0xFFF);

			if (imm & 0x800)
				imm |= ~0xFFFLL;

			processor->gpr[rd] = (s64)processor->gpr[rs1] + imm;

			return success;
		}
		case HX_MN_SUB: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = (s64)processor->gpr[rs1] - (s64)processor->gpr[rs2];

			return success;
		}
		case HX_MN_MUL: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] * processor->gpr[rs2];

			return success;
		}
		case HX_MN_DIV: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] / processor->gpr[rs2];

			return success;
		}
		case HX_MN_REM: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] % processor->gpr[rs2];

			return success;
		}
		case HX_MN_AND: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] & processor->gpr[rs2];

			return success;
		}
		case HX_MN_ANDI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] & imm;

			return success;
		}
		case HX_MN_OR: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] | processor->gpr[rs2];

			return success;
		}
		case HX_MN_ORI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] | imm;

			return success;
		}
		case HX_MN_XOR: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] ^ processor->gpr[rs2];

			return success;
		}
		case HX_MN_XORI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] ^ imm;

			return success;
		}
		case HX_MN_SLL: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] << processor->gpr[rs2];

			return success;
		}
		case HX_MN_SLLI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] << imm;

			return success;
		}
		case HX_MN_SLR: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] >> processor->gpr[rs2];

			return success;
		}
		case HX_MN_SLRI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] >> imm;

			return success;
		}
		case HX_MN_SAR: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = (u64)((s64)processor->gpr[rs1] >> processor->gpr[rs2]);

			return success;
		}
		case HX_MN_SARI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = (u64)((s64)processor->gpr[rs1] >> imm);

			return success;
		}
		case HX_MN_SLT: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] =
				((s64)processor->gpr[rs1] < (s64)processor->gpr[rs2]);

			return success;
		}
		case HX_MN_SLTI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			s64 imm = instruction->operands[2].value.imm;
			s64 rs1val = (s64)processor->gpr[rs1];
			u64 result = (rs1val < imm) ? 1 : 0;

			processor->gpr[rd] = result;

			return success;
		}
		case HX_MN_SLTU: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;

			processor->gpr[rd] = processor->gpr[rs1] < processor->gpr[rs2];

			return success;
		}
		case HX_MN_SLTUI: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->gpr[rs1] < imm;

			return success;
		}

		case HX_MN_LB: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read8(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LQ: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read16(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LH: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read32(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LW: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read64(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LBU: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read8(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LQU: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read16(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_LHU: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.memory.reg;
			u16 imm = instruction->operands[1].value.memory.offset;

			processor->gpr[rd] = cache_read32(&processor->l1d, memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_MN_SB: {
			u8 rd = instruction->operands[0].value.memory.reg;
			u8 imm = instruction->operands[0].value.memory.offset;
			u16 rs1 = instruction->operands[1].value.reg;

			cache_write8(&processor->l1d, memory, processor->gpr[rd] + imm, processor->gpr[rs1]);


			return success;
		}
		case HX_MN_SQ: {
			u8 rd = instruction->operands[0].value.memory.reg;
			u8 imm = instruction->operands[0].value.memory.offset;
			u16 rs1 = instruction->operands[1].value.reg;

			cache_write16(&processor->l1d, memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_MN_SH: {
			u8 rd = instruction->operands[0].value.memory.reg;
			u8 imm = instruction->operands[0].value.memory.offset;
			u16 rs1 = instruction->operands[1].value.reg;

			cache_write32(&processor->l1d, memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_MN_SW: {
			u8 rd = instruction->operands[0].value.memory.reg;
			u8 imm = instruction->operands[0].value.memory.offset;
			u16 rs1 = instruction->operands[1].value.reg;

			cache_write64(&processor->l1d, memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_MN_BEQ: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;
			
			if (processor->gpr[rs1] == processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}
		case HX_MN_BNE: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			if (processor->gpr[rs1] != processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}
		case HX_MN_BLT: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			if (processor->gpr[rs1] < processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}
		case HX_MN_BGE: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			if (processor->gpr[rs1] >= processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}
		case HX_MN_BLTU: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			if (processor->gpr[rs1] < processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}
		case HX_MN_BGEU: {
			u8 rs1 = instruction->operands[0].value.reg;
			u8 rs2 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			if (processor->gpr[rs1] >= processor->gpr[rs2]){
				processor->pc = imm;
			}

			return success;
		}

		case HX_MN_JAL: {
			u8 rd = instruction->operands[0].value.reg;
			u32 imm = instruction->operands[1].value.imm;

			if (rd != 0) {
				processor->gpr[rd] = processor->pc;
			}

			processor->pc = imm;

			return success;
		}
		case HX_MN_JALR: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u16 imm = instruction->operands[2].value.imm;

			processor->gpr[rd] = processor->pc;

			processor->pc = processor->gpr[rs1] + imm;

			return success;
		}
		case HX_MN_SCALL: {
			/* TODO: Jump to system table */

			return success;
		}
		case HX_MN_STRAP: {
			/* TODO: Jump to trap table */

			return success;
		}
		case HX_MN_SRET: {
			/* TODO: Jump back to user space */

			return success;
		}
		case HX_MN_WFI: {
			/* TODO: Wait for interrupt */

			processor->halted = 1;

			return success;
		}

		case HX_MN_MOVZ: {
			u8 rd = instruction->operands[0].value.reg;
			u64 imm = instruction->operands[1].value.imm;
			u8 shift = instruction->operands[2].value.imm;

			processor->gpr[rd] = (imm & 0xFFFFULL) << (shift * 16);

			return success;
		}
		case HX_MN_MOVP: {
			u8 rd = instruction->operands[0].value.reg;
			u64 imm = instruction->operands[1].value.imm;
			u8 shift = instruction->operands[2].value.imm;

			u64 mask = 0xFFFFULL << (shift * 16);

			processor->gpr[rd] =
				(processor->gpr[rd] & ~mask) |
				((imm & 0xFFFFULL) << (shift * 16));

			return success;
		}
		case HX_MN_MOVN: {
			u8 rd = instruction->operands[0].value.reg;
			u64 imm = instruction->operands[1].value.imm;
			u8 shift = instruction->operands[2].value.imm;

			processor->gpr[rd] =
				~((imm & 0xFFFFULL) << (shift * 16));

			return success;
		}
		case HX_MN_MOV: {
			u8 rd = instruction->operands[0].value.reg;
			u16 imm = instruction->operands[1].value.imm;

			processor->gpr[rd] = (s16)imm;

			return success;
		}
		case HX_MN_CSL: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;
			u16 imm = instruction->operands[3].value.imm;

			u64 x = processor->gpr[rs1];
			u64 y = processor->gpr[rs2];

			switch (imm) {
				case 0: // eq
					processor->gpr[rd] = (x == y) ? x : y;
					break;
				case 1: // ne
					processor->gpr[rd] = (x != y) ? x : y;
					break;
				case 2: // lt
					processor->gpr[rd] =
						((s64)x < (s64)y) ? x : y;
					break;
				case 3: // ge
					processor->gpr[rd] =
						((s64)x >= (s64)y) ? x : y;
					break;
				case 4: // ltu
					processor->gpr[rd] =
						(x < y) ? x : y;
					break;
				case 5: // geu
					processor->gpr[rd] =
						(x >= y) ? x : y;
					break;
				default:
					return failure;
			}

			return success;
		}
		case HX_MN_CSINC: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;
			u16 imm = instruction->operands[3].value.imm;

			u64 x = processor->gpr[rs1];
			u64 y = processor->gpr[rs2];

			b8 condition;

			switch (imm) {
				case 0: // eq
					condition = (x == y);
					break;
				case 1: // ne
					condition = (x != y);
					break;
				case 2: // lt
					condition = ((s64)x < (s64)y);
					break;
				case 3: // ge
					condition = ((s64)x >= (s64)y);
					break;
				case 4: // ltu
					condition = (x < y);
					break;
				case 5: // geu
					condition = (x >= y);
					break;
				default:
					return failure;
			}

			processor->gpr[rd] = condition ? x + 1 : y;

			return success;
		}
		case HX_MN_CSNEG: {
			u8 rd = instruction->operands[0].value.reg;
			u8 rs1 = instruction->operands[1].value.reg;
			u8 rs2 = instruction->operands[2].value.reg;
			u16 imm = instruction->operands[3].value.imm;

			u64 x = processor->gpr[rs1];
			u64 y = processor->gpr[rs2];

			b8 condition;

			switch (imm) {
				case 0: // eq
					condition = (x == y);
					break;
				case 1: // ne
					condition = (x != y);
					break;
				case 2: // lt
					condition = ((s64)x < (s64)y);
					break;
				case 3: // ge
					condition = ((s64)x >= (s64)y);
					break;
				case 4: // ltu
					condition = (x < y);
					break;
				case 5: // geu
					condition = (x >= y);
					break;
				default:
					return failure;
			}

			processor->gpr[rd] = condition ? (u64)-(s64)x : y;

			return success;
		}

		default:
			return failure;
	}
}


b8 processor_run(hx_processor *processor, hx_memory *memory, hx_cli *cli)
{
	if (processor == NULL || memory == NULL || cli == NULL) {
		return failure;
	}

	u32 program_size = fread(memory->memory, 1, MAX_MEMORY, cli->input_file);

	while (!processor->halted) {
		u32 encoded;
		hx_instruction instruction;

		if (processor->pc > program_size - 4) {
			fprintf(stderr, "PC out of bounds: 0x%08lx\n", processor->pc);
			return failure;
		}

		encoded = processor_fetch32(processor, memory);

		instruction = instruction_decode(encoded);

		if (!processor_execute(processor, memory, &instruction)) {
			return failure;
		}

		processor->gpr[0] = 0;

	}

	return success;
}
