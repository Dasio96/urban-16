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
  default:
    break;
  }
}
