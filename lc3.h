#ifndef LC3_H
#define LC3_H

#include <stdint.h>

enum {
  FL_POS = 1 << 0,
  FL_ZRO = 1 << 1,
  FL_NEG = 1 << 2,
};

enum {
  OP_BR = 0,
  OP_ADD,
  OP_LD,
  OP_ST,
  OP_JSR,
  OP_AND,
  OP_LDR,
  OP_STR,
  OP_RTI,
  OP_NOT,
  OP_LDI,
  OP_STI,
  OP_JMP,
  OP_RES,
  OP_LEA,
  OP_TRAP
};

typedef enum {
  TRAP_GETC = 0x20,
  TRAP_OUT = 0x21,
  TRAP_PUTS = 0x22,
  TRAP_IN = 0x23,
  TRAP_PUTSP = 0x24,
  TRAP_HALT = 0x25
} lc3_trap_code;

typedef struct {
  uint16_t ram[65536];
  uint16_t reg[8];
  uint16_t pc;
  uint16_t cond;
  int running;
} lc3_cpu;

void lc3_init(lc3_cpu *cpu);
void lc3_step(lc3_cpu *cpu);
void update_flags(lc3_cpu *cpu, uint16_t r);

uint16_t sign_extend(uint16_t x, int bit_count);

#endif // !LC3_H
