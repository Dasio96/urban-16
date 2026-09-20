#include "lc3.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("%s\n", argv[0]);
    return 1;
  }

  lc3_cpu cpu;
  lc3_init(&cpu);

  if (!lc3_load_image(&cpu, argv[1])) {
    printf("failed to load image %s\n", argv[1]);
    return 1;
  }

  while (cpu.running) {
    lc3_step(&cpu);
  }

  return 0;
}
