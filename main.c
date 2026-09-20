#include "lc3.h"
#include <stdio.h>

int main(void) {
  lc3_cpu cpu;
  lc3_init(&cpu);

  uint16_t start_address = 0x3000;

  cpu.ram[start_address + 0] = 0xE002;
  cpu.ram[start_address + 1] = 0xF022;
  cpu.ram[start_address + 2] = 0xF025;

  cpu.ram[start_address + 3] = 'U';
  cpu.ram[start_address + 4] = 'R';
  cpu.ram[start_address + 5] = 'B';
  cpu.ram[start_address + 6] = 'A';
  cpu.ram[start_address + 7] = 'N';
  cpu.ram[start_address + 8] = '\n';
  cpu.ram[start_address + 9] = '\0';

  while (cpu.running) {
    lc3_step(&cpu);
  }

  return 0;
}
