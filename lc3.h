#ifndef LC3_H
#define LC3_H

#include <stdint.h>

enum {
  FL_Pos = 1 << 0,
  FL_ZRO = 1 << 1,
  FL_NEG = 1 << 2,
};

typedef struct {
  uint16_t ram[65536];
  uint16_t reg[8];
  uint16_t pc;
  uint16_t cond;
} lc3_cpu;

#endif // !LC3_H
