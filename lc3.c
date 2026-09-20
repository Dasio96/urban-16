#include "lc3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

static int check_key(void) {
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  struct timeval timeout = {0, 0};
  return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0;
}

uint16_t mem_read(lc3_cpu *cpu, uint16_t address) {
  if (address == MEM_KBSR) {
    if (check_key()) {
      cpu->ram[MEM_KBSR] = (1 << 15);
      cpu->ram[MEM_KBDR] = (uint16_t)getchar();
    } else {
      cpu->ram[MEM_KBSR] = 0;
    }
  }
  return cpu->ram[address];
}

void mem_write(lc3_cpu *cpu, uint16_t address, uint16_t val) {
  cpu->ram[address] = val;
}

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
  }
  return x;
}

void update_flags(lc3_cpu *cpu, uint16_t r) {
  if (cpu->reg[r] == 0) {
    cpu->cond_flags = FL_ZRO;
  } else if (cpu->reg[r] >> 15) {
    cpu->cond_flags = FL_NEG;
  } else {
    cpu->cond_flags = FL_POS;
  }
}

void lc3_init(lc3_cpu *cpu) {
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->reg, 0, sizeof(cpu->reg));
  cpu->pc = PC_START;
  cpu->cond_flags = FL_ZRO;
  cpu->running = 1;
}

int lc3_load_image(lc3_cpu *cpu, const char *image_path) {
  FILE *file = fopen(image_path, "rb");
  if (!file) {
    return 0;
  }
  uint8_t buf[2];
  if (fread(buf, 1, 2, file) != 2) {
    fclose(file);
    return 0;
  }
  uint16_t origin = (buf[0] << 8) | buf[1];
  uint32_t current_addr = origin;
  while (fread(buf, 1, 2, file) == 2) {
    if (current_addr >= RAM_SIZE) {
      fclose(file);
      return 0;
    }
    uint16_t word = (buf[0] << 8) | buf[1];
    cpu->ram[current_addr++] = word;
  }
  fclose(file);
  return 1;
}

static void op_add(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t sr1 = (instr >> 6) & 0x7;
  uint16_t imm_flag = (instr >> 5) & 0x1;
  if (imm_flag) {
    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
    cpu->reg[dr] = cpu->reg[sr1] + imm5;
  } else {
    uint16_t sr2 = instr & 0x7;
    cpu->reg[dr] = cpu->reg[sr1] + cpu->reg[sr2];
  }
  update_flags(cpu, dr);
}

static void op_and(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t sr1 = (instr >> 6) & 0x7;
  uint16_t imm_flag = (instr >> 5) & 0x1;
  if (imm_flag) {
    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
    cpu->reg[dr] = cpu->reg[sr1] & imm5;
  } else {
    uint16_t sr2 = instr & 0x7;
    cpu->reg[dr] = cpu->reg[sr1] & cpu->reg[sr2];
  }
  update_flags(cpu, dr);
}

static void op_not(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t sr = (instr >> 6) & 0x7;
  cpu->reg[dr] = ~cpu->reg[sr];
  update_flags(cpu, dr);
}

static void op_br(lc3_cpu *cpu, uint16_t instr) {
  uint16_t cond = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  if (cond & cpu->cond_flags) {
    cpu->pc += pc_offset;
  }
}

static void op_jmp(lc3_cpu *cpu, uint16_t instr) {
  uint16_t base_r = (instr >> 6) & 0x7;
  cpu->pc = cpu->reg[base_r];
}

static void op_jsr(lc3_cpu *cpu, uint16_t instr) {
  uint16_t long_flag = (instr >> 11) & 0x1;
  if (long_flag) {
    uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
    cpu->reg[7] = cpu->pc;
    cpu->pc += pc_offset;
  } else {
    uint16_t base_r = (instr >> 6) & 0x7;
    uint16_t target = cpu->reg[base_r];
    cpu->reg[7] = cpu->pc;
    cpu->pc = target;
  }
}

static void op_ld(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  cpu->reg[dr] = mem_read(cpu, cpu->pc + pc_offset);
  update_flags(cpu, dr);
}

static void op_ldi(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  uint16_t ptr = mem_read(cpu, cpu->pc + pc_offset);
  cpu->reg[dr] = mem_read(cpu, ptr);
  update_flags(cpu, dr);
}

static void op_ldr(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t base_r = (instr >> 6) & 0x7;
  uint16_t offset = sign_extend(instr & 0x3F, 6);
  cpu->reg[dr] = mem_read(cpu, cpu->reg[base_r] + offset);
  update_flags(cpu, dr);
}

static void op_lea(lc3_cpu *cpu, uint16_t instr) {
  uint16_t dr = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  cpu->reg[dr] = cpu->pc + pc_offset;
  update_flags(cpu, dr);
}

static void op_st(lc3_cpu *cpu, uint16_t instr) {
  uint16_t sr = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  mem_write(cpu, cpu->pc + pc_offset, cpu->reg[sr]);
}

static void op_sti(lc3_cpu *cpu, uint16_t instr) {
  uint16_t sr = (instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
  uint16_t ptr = mem_read(cpu, cpu->pc + pc_offset);
  mem_write(cpu, ptr, cpu->reg[sr]);
}

static void op_str(lc3_cpu *cpu, uint16_t instr) {
  uint16_t sr = (instr >> 9) & 0x7;
  uint16_t base_r = (instr >> 6) & 0x7;
  uint16_t offset = sign_extend(instr & 0x3F, 6);
  mem_write(cpu, cpu->reg[base_r] + offset, cpu->reg[sr]);
}

static void op_trap(lc3_cpu *cpu, uint16_t instr) {
  cpu->reg[7] = cpu->pc;
  uint16_t trapvec = instr & 0xFF;
  switch (trapvec) {
  case TRAP_GETC: {
    int c = getchar();
    if (c == EOF) {
      cpu->reg[0] = 0;
      cpu->running = 0;
    } else {
      cpu->reg[0] = (uint16_t)c;
      update_flags(cpu, 0);
    }
    break;
  }
  case TRAP_OUT:
    putc((char)cpu->reg[0], stdout);
    fflush(stdout);
    break;
  case TRAP_PUTS: {
    uint16_t addr = cpu->reg[0];
    while (1) {
      uint16_t c = mem_read(cpu, addr++);
      if (c == 0x0000)
        break;
      putc((char)c, stdout);
    }
    fflush(stdout);
    break;
  }
  case TRAP_IN: {
    printf("Enter a character: ");
    fflush(stdout);
    int c = getchar();
    if (c == EOF) {
      cpu->reg[0] = 0;
      cpu->running = 0;
    } else {
      putc(c, stdout);
      fflush(stdout);
      cpu->reg[0] = (uint16_t)c;
      update_flags(cpu, 0);
    }
    break;
  }
  case TRAP_PUTSP: {
    uint16_t addr = cpu->reg[0];
    while (1) {
      uint16_t word = mem_read(cpu, addr++);
      if (word == 0x0000)
        break;
      char c1 = (char)(word & 0xFF);
      if (c1 == '\0')
        break;
      putc(c1, stdout);
      char c2 = (char)(word >> 8);
      if (c2 != '\0')
        putc(c2, stdout);
    }
    fflush(stdout);
    break;
  }
  case TRAP_HALT:
    puts("\n--- HALT ---");
    fflush(stdout);
    cpu->running = 0;
    break;
  default:
    cpu->running = 0;
    break;
  }
}

static void op_illegal(lc3_cpu *cpu, uint16_t instr) {
  (void)instr;
  fprintf(stderr, "Illegal instruction\n");
  cpu->running = 0;
}

void lc3_step(lc3_cpu *cpu) {
  if (!cpu->running)
    return;
  uint16_t instr = mem_read(cpu, cpu->pc++);
  uint16_t opcode = instr >> 12;
  switch (opcode) {
  case OP_ADD:
    op_add(cpu, instr);
    break;
  case OP_AND:
    op_and(cpu, instr);
    break;
  case OP_NOT:
    op_not(cpu, instr);
    break;
  case OP_BR:
    op_br(cpu, instr);
    break;
  case OP_JMP:
    op_jmp(cpu, instr);
    break;
  case OP_JSR:
    op_jsr(cpu, instr);
    break;
  case OP_LD:
    op_ld(cpu, instr);
    break;
  case OP_LDI:
    op_ldi(cpu, instr);
    break;
  case OP_LDR:
    op_ldr(cpu, instr);
    break;
  case OP_LEA:
    op_lea(cpu, instr);
    break;
  case OP_ST:
    op_st(cpu, instr);
    break;
  case OP_STI:
    op_sti(cpu, instr);
    break;
  case OP_STR:
    op_str(cpu, instr);
    break;
  case OP_TRAP:
    op_trap(cpu, instr);
    break;
  case OP_RTI:
  case OP_RES:
  default:
    op_illegal(cpu, instr);
    break;
  }
}
