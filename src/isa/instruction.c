#include "isa/instruction.h"
#include "isa/mnemonic.h"
#include "isa/operand.h"
#include <stdio.h>
#include <stdlib.h>

hx_instruction instruction_decode(u32 encoded)
{
	hx_instruction instruction;

	u8 opcode = encoded & 0x7F;

	/* functions */
	switch (opcode) {
		case 0x27: {
			u8 funct3 = (encoded >> 12) & 0x07;
			u8 funct7 = (encoded >> 25) & 0x7F;
			u8 rd = (encoded >> 7) & 0x1F;
			u8 rs1 = (encoded >> 15) & 0x1F;
			u8 rs2 = (encoded >> 20) & 0x1F;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, funct7);
			instruction.operand_count = 3;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs1;
			instruction.operands[2].type = HX_OPERAND_REGISTER;
			instruction.operands[2].value.reg = rs2;
			return instruction;
		}
		case 0x77: {
			u8 funct3 = (encoded >> 12) & 0x07;
			u8 funct7 = (encoded >> 25) & 0x7F;
			u8 rd = (encoded >> 7) & 0x1F;
			u8 rs1 = (encoded >> 15) & 0x1F;
			u8 rs2 = (encoded >> 20) & 0x1F;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, funct7);
			instruction.operand_count = 4;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs1;
			instruction.operands[2].type = HX_OPERAND_REGISTER;
			instruction.operands[2].value.reg = rs2;
			instruction.operands[3].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[3].value.imm = funct7;
			return instruction;
		}
		case 0x03: {
			u8 funct3 = (encoded >> 12) & 0x07;
			u8 rd = (encoded >> 7) & 0x1F;
			u8 rs1 = (encoded >> 15) & 0x1F;
			u16 imm = (encoded >> 20) & 0xFFF;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
			instruction.operand_count = 2;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_MEMORY;
			instruction.operands[1].value.memory.reg = rs1;
			instruction.operands[1].value.memory.offset = imm;
			return instruction;
		}
		case 0x07: {
			u8 funct3 = (encoded >> 12) & 0x07;
			u8 rd = (encoded >> 7) & 0x1F;
			u8 rs1 = (encoded >> 15) & 0x1F;
			u16 imm = (encoded >> 20) & 0xFFF;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
			instruction.operand_count = 3;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs1;
			instruction.operands[2].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[2].value.imm = imm;
			return instruction;
		}
		case 0x63: {
			u8 funct3 = (encoded >> 12) & 0x07;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
			instruction.operand_count = 0;
			return instruction;
		}
		case 0x73: {
			u8 rd = (encoded >> 7) & 0x1F;
			u8 rs1 = (encoded >> 15) & 0x1F;
			u16 imm = (encoded >> 20) & 0xFFF;
			instruction.mnemonic = HX_MN_JALR;
			instruction.operand_count = 3;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs1;
			instruction.operands[2].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[2].value.imm = imm;
			return instruction;
		}
		case 0x23: {
			u8 funct3 = (encoded >> 7) & 0x07;
			u8 rs1 = (encoded >> 10) & 0x1F;
			u8 rs2 = (encoded >> 15) & 0x1F;
			u16 imm = (encoded >> 20) & 0xFFF;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
			instruction.operand_count = 2;
			instruction.operands[0].type = HX_OPERAND_MEMORY;
			instruction.operands[0].value.memory.reg = rs1;
			instruction.operands[0].value.memory.offset = imm;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs2;
			return instruction;
		}
		case 0x4F: {
			u8 funct3 = (encoded >> 7) & 0x07;
			u8 rs1 = (encoded >> 10) & 0x1F;
			u8 rs2 = (encoded >> 15) & 0x1F;
			u16 imm = (encoded >> 20) & 0xFFF;
			instruction.mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
			instruction.operand_count = 3;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rs1;
			instruction.operands[1].type = HX_OPERAND_REGISTER;
			instruction.operands[1].value.reg = rs2;
			instruction.operands[2].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[2].value.imm = (imm << 1);
			return instruction;
		}
		case 0x6F: {
			u8 rd = (encoded >> 7) & 0x1F;
			u16 imm = (encoded >> 12) & 0xFFFFF;
			instruction.mnemonic = HX_MN_JAL;
			instruction.operand_count = 2;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[1].value.imm = (imm << 1);
			return instruction;
		}
		case 0x43: {
			u8 rd = (encoded >> 7) & 0x1F;
			u16 imm = (encoded >> 16) & 0xFFFF;
			u8 mod = (encoded >> 12) & 0x03;
			u8 shift = (encoded >> 14) & 0x03;
			instruction.mnemonic = mnemonic_from_encoding(opcode, mod, 0);
			instruction.operand_count = 3;
			instruction.operands[0].type = HX_OPERAND_REGISTER;
			instruction.operands[0].value.reg = rd;
			instruction.operands[1].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[1].value.imm = imm;
			instruction.operands[2].type = HX_OPERAND_IMMEDIATE;
			instruction.operands[2].value.imm = shift;
			return instruction;
		}
		default:
			fprintf(stderr, "invalid instruction opcode\n");
			exit(1);
	}
}
