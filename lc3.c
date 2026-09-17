#include "lc3.h"
#include <string.h>

void lc3_init(lc3_cpu *cpu) {
  memset(cpu, 0, sizeof(lc3_cpu));
  cpu->pc = 0x3000;
  cpu->cond = FL_ZRO;
}
