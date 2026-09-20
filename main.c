#include "lc3.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    return 1;
  }
  lc3_cpu *cpu = malloc(sizeof(*cpu));
  if (!cpu) {
    return 1;
  }
  lc3_init(cpu);
  if (!lc3_load_image(cpu, argv[1])) {
    free(cpu);
    return 1;
  }
  while (cpu->running) {
    lc3_step(cpu);
  }
  free(cpu);
  return 0;
}
