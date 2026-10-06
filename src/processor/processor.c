#include "processor/processor.h"

#include "processor/memory.h"
#include "types.h"

#include <endian.h>
#include <isac/instruction.h>
#include <isac/operand.h>
#include <isac/format.h>
#include <stdio.h>

static b8 jumped = false;

b8 processor_create(hx_processor *processor)
{
	if (processor == NULL)
		return failure;


	return processor_reboot(processor);
}

b8 processor_destroy(hx_processor *processor)
{
	if (processor == NULL)
		return failure;

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

	return success;
}

static u32 processor_fetch(hx_processor *processor, hx_memory *memory)
{
	u32 instruction;

	instruction = memory_read32(memory, processor->pc);

	return instruction;
}

static b8 processor_execute(hx_processor *processor, hx_memory *memory, hx_instruction *instruction)
{
	switch (instruction_mnemonic(instruction)) {
		case HX_ADD: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] + processor->gpr[rs2];

			return success;
		}
		case HX_ADDI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			if (imm & 0x800)
				imm |= ~0xFFFLL;

			processor->gpr[rd] = (s64)processor->gpr[rs1] + imm;

			return success;
		}
		case HX_SUB: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = (s64)processor->gpr[rs1] - (s64)processor->gpr[rs2];

			return success;
		}
		case HX_AND: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] & processor->gpr[rs2];

			return success;
		}
		case HX_ANDI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] & imm;

			return success;
		}
		case HX_OR: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] | processor->gpr[rs2];

			return success;
		}
		case HX_ORI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] | imm;

			return success;
		}
		case HX_XOR: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] ^ processor->gpr[rs2];

			return success;
		}
		case HX_XORI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] ^ imm;

			return success;
		}
		case HX_SLL: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] << processor->gpr[rs2];

			return success;
		}
		case HX_SLLI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] << imm;

			return success;
		}
		case HX_SLR: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] >> processor->gpr[rs2];

			return success;
		}
		case HX_SLRI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] >> imm;

			return success;
		}
		case HX_SAR: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = (u64)((s64)processor->gpr[rs1] >> processor->gpr[rs2]);

			return success;
		}
		case HX_SARI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = (u64)((s64)processor->gpr[rs1] >> imm);

			return success;
		}
		case HX_SLT: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] =
				((s64)processor->gpr[rs1] < (s64)processor->gpr[rs2]);

			return success;
		}
		case HX_SLTI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));
			s64 rs1val = (s64)processor->gpr[rs1];
			u64 result = (rs1val < imm) ? 1 : 0;

			processor->gpr[rd] = result;

			return success;
		}
		case HX_SLTU: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER));

			processor->gpr[rd] = processor->gpr[rs1] < processor->gpr[rs2];

			return success;
		}
		case HX_SLTUI: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->gpr[rs1] < (u64)imm;

			return success;
		}

		case HX_LB: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read8(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LQ: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read16(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LH: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read32(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LW: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read64(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LBU: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read8(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LQU: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read16(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_LHU: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY));
			s16 imm = operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY));

			processor->gpr[rd] = memory_read32(memory, processor->gpr[rs1] + imm);

			return success;
		}
		case HX_SB: {
			u8 rd = operand_get_memory_register(instruction_get_operand(instruction, 0, HX_MEMORY));
			u8 imm = operand_get_memory_offset(instruction_get_operand(instruction, 0, HX_MEMORY));
			s16 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));

			memory_write8(memory, processor->gpr[rd] + imm, processor->gpr[rs1]);


			return success;
		}
		case HX_SQ: {
			u8 rd = operand_get_memory_register(instruction_get_operand(instruction, 0, HX_MEMORY));
			u8 imm = operand_get_memory_offset(instruction_get_operand(instruction, 0, HX_MEMORY));
			s16 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));

			memory_write16(memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_SH: {
			u8 rd = operand_get_memory_register(instruction_get_operand(instruction, 0, HX_MEMORY));
			u8 imm = operand_get_memory_offset(instruction_get_operand(instruction, 0, HX_MEMORY));
			s16 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));

			memory_write32(memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_SW: {
			u8 rd = operand_get_memory_register(instruction_get_operand(instruction, 0, HX_MEMORY));
			u8 imm = operand_get_memory_offset(instruction_get_operand(instruction, 0, HX_MEMORY));
			s16 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));

			memory_write64(memory, processor->gpr[rd] + imm, processor->gpr[rs1]);

			return success;
		}
		case HX_BEQ: {
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));
			
			if (processor->gpr[rs1] == processor->gpr[rs2]){
				processor->pc = processor->pc + imm;
				jumped = true;
			}

			return success;
		}
		case HX_BNE: {
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			if (processor->gpr[rs1] != processor->gpr[rs2]){
				processor->pc = processor->pc + imm;
				jumped = true;
			}

			return success;
		}
		case HX_BLT: {
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			if (processor->gpr[rs1] < processor->gpr[rs2]){
				processor->pc = processor->pc + imm;
			}

			return success;
		}
		case HX_BGE: {
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs2 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			if (processor->gpr[rs1] >= processor->gpr[rs2]){
				processor->pc = processor->pc + imm;
				jumped = true;
			}

			return success;
		}

		case HX_JAL: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 1, HX_IMMEDIATE));

			if (rd != 0) {
				processor->gpr[rd] = processor->pc;
			}

			processor->pc = processor->pc + imm;
			jumped = true;

			return success;
		}
		case HX_JRAL: {
			u8 rd = operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER));
			u8 rs1 = operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER));
			s64 imm = operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE));

			processor->gpr[rd] = processor->pc;

			processor->pc = processor->gpr[rs1] + imm;
			jumped = true;

			return success;
		}

		default:
			return failure;
	}
}


b8 processor_run(hx_processor *processor, hx_memory *memory, FILE *in)
{
	if (processor == NULL || memory == NULL) {
		return failure;
	}

	u32 program_size = fread(memory->memory, 1, MAX_MEMORY, in);

	printf("START\n");
	while (true) {
		u32 encoded;
		hx_instruction *instruction;

		if (processor->pc == program_size) {
			printf("END\n");
			return success;
		}
		if (processor->pc > program_size - 4) {
			fprintf(stderr, "PC out of bounds: 0x%08lx\n", processor->pc);
			return failure;
		}

		encoded = processor_fetch(processor, memory);

		switch ((encoded >> 2) & 0x1F) {
			case 0x00:
				instruction = format_r_decode(encoded);
				break;
			case 0x08:
			case 0x09:
			case 0x0A:
				instruction = format_i_decode(encoded);
				break;
			case 0x01:
				instruction = format_s_decode(encoded);
				break;
			case 0x02:
				instruction = format_j_decode(encoded);
				break;
			case 0x10:
				instruction = format_b_decode(encoded);
				break;
		}

		if (!processor_execute(processor, memory, instruction)) {
			return failure;
		}

		if (!jumped)
			processor->pc = processor->pc + 4;

		jumped = false;

		printf("0x%08lx: %s\n", processor->pc, mnemonic_string(instruction_mnemonic(instruction)));
	}

	return success;
}
