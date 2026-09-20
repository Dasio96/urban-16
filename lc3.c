#include "lc3.h"
#include <string.h>

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
  }
  return x;
}

void update_flags(lc3_cpu *cpu, uint16_t r) {
  if (cpu->reg[r] == 0)
    cpu->cond = FL_ZRO;
  else if ((cpu->reg[r] >> 15) == 1)
    cpu->cond = FL_NEG;
  else
    cpu->cond = FL_POS;
}

void lc3_init(lc3_cpu *cpu) {
  memset(cpu, 0, sizeof(lc3_cpu));
  cpu->pc = 0x3000;
  cpu->cond = FL_ZRO;
}

void lc3_step(lc3_cpu *cpu) {
  uint16_t instr = cpu->ram[cpu->pc];
  cpu->pc++;
  uint16_t opcode = instr >> 12;

  switch (opcode) {
  case OP_ADD: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t sr1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag == 0) {
      uint16_t sr2 = instr & 0x7;
      cpu->reg[dr] = cpu->reg[sr1] + cpu->reg[sr2];
    } else {
      uint16_t imm5 = sign_extend(instr & 0x1F, 5);
      cpu->reg[dr] = cpu->reg[sr1] + imm5;
    }

    update_flags(cpu, dr);
    break;
  }

  case OP_AND: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t sr1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag == 0) {
      uint16_t sr2 = instr & 0x7;
      cpu->reg[dr] = cpu->reg[sr1] & cpu->reg[sr2];
    } else {
      uint16_t imm5 = sign_extend(instr & 0x1F, 5);
      cpu->reg[dr] = cpu->reg[sr1] & imm5;
    }

    update_flags(cpu, dr);
    break;
  }

  case OP_NOT: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t sr1 = (instr >> 6) & 0x7;

    cpu->reg[dr] = ~cpu->reg[sr1];

    update_flags(cpu, dr);
    break;
  }

  case OP_BR: {
    uint16_t cond_flgas = (instr >> 9) & 0x7;
    uint16_t offset9 = sign_extend(instr & 0x1FF, 9);

    if ((cond_flgas & cpu->cond) != 0)
      cpu->pc += offset9;

    break;
  }

  case OP_JMP: {
    uint16_t baseR = (instr >> 6) & 0x7;
    cpu->pc = cpu->reg[baseR];
    break;
  }

  case OP_LD: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t PCoffset9 = sign_extend(instr & 0x1FF, 9);
    uint16_t address = cpu->pc + PCoffset9;

    cpu->reg[dr] = cpu->ram[address];
    break;
  }

  case OP_ST: {
    uint16_t sr = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint16_t address = cpu->pc + pc_offset;

    cpu->ram[address] = cpu->reg[sr];
    break;
  }

  case OP_LDR: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t baseR = (instr >> 6) & 0x7;
    uint16_t offset6 = sign_extend(instr & 0x3F, 6);
    uint16_t address = cpu->reg[baseR] + offset6;

    cpu->reg[dr] = cpu->ram[address];
    update_flags(cpu, dr);
    break;
  }

  case OP_STR: {
    uint16_t sr = (instr >> 9) & 0x7;
    uint16_t baseR = (instr >> 6) & 0x7;
    uint16_t offset6 = sign_extend(instr & 0x3F, 6);
    uint16_t address = cpu->reg[baseR] + offset6;

    cpu->ram[address] = cpu->reg[sr];
    break;
  }

  case OP_LDI: {
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint16_t ptr = cpu->pc + pc_offset;
    uint16_t real_address = cpu->ram[ptr];

    cpu->reg[dr] = cpu->ram[real_address];

    update_flags(cpu, dr);
    break;
  }

  case OP_STI: {
    uint16_t sr = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint16_t ptr = cpu->pc + pc_offset;
    uint16_t real_address = cpu->ram[ptr];

    cpu->ram[real_address] = cpu->reg[sr];

    break;
  }

  default:
    break;
  }
}
