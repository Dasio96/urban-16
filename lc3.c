#include "lc3.h"
#include <string.h>

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
  case OP_ADD:
    break;
  default:
    break;
  }
}
