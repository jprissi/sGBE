#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <fstream>

#include "memory.hpp"

#define InterruptFlagAddress 0xFF0F
#define InterruptEnableAddress 0xFFFF

#define INT_VBLANK 1 << 0
#define INT_JOYPAD 1 << 5

#define CONST_ZERO_FLAG (1 << 7)
#define CONST_SUBSTRACT_FLAG (1 << 6)
#define CONST_HALFCARRY_FLAG (1 << 5)
#define CONST_CARRY_FLAG (1 << 4)

class CPU
{

private:
  int32_t total_cycles = 0;
  uint16_t breakpoint = 0xffff;
  void default_init();
  void assert_init();

  void handle_interrupts();
  bool interrupts_enabled = false; // IME flag, disabled at start

public:
  uint16_t BC, DE, HL, AF;
  uint8_t *p_F = (uint8_t *)&AF;
  uint8_t *p_A = p_F + 1;

  uint8_t *p_C = (uint8_t *)&BC;
  uint8_t *p_B = p_C + 1;

  uint8_t *p_E = (uint8_t *)&DE;
  uint8_t *p_D = p_E + 1;

  uint8_t *p_L = (uint8_t *)&HL;
  uint8_t *p_H = p_L + 1;

  uint8_t *flags = p_F; // [Z, N, H, C, 0, 0, 0, 0] 8 bits, the same as F register

  uint16_t SP = 0xFFE;
  uint16_t PC = 0x0000; // Before boot

  uint8_t *registers[8] = {this->p_B, this->p_C, this->p_D, this->p_E, this->p_H, this->p_L, reinterpret_cast<uint8_t *>(&this->HL), this->p_A};

  uint16_t *registers16[4] = {
      &this->BC,
      &this->DE,
      &this->HL,
      &this->SP,
  };
  uint16_t *registers16_2[4] = {
      &this->BC,
      &this->DE,
      &this->HL,
      &this->AF,
  };

  MemoryController m;

  CPU(std::string cartridge_path, bool debug_implementation = false);

  uint8_t next_instruction_relative_pos;
  uint8_t current_opcode;

  void set_breakpoint(uint16_t address);

  void log(std::ofstream &file);
  bool step_by_step = false;

  bool disable_interrupts_next = false; // Or use a counter instead
  void enable_interrupts();
  void disable_interrupts();
  void request_interrupt(uint8_t interrupt);

  // 7 6 5 4 3 2 1 0
  // - - - J S T L V
  // J - Joypad, S - Serial, T - Timer, L - LCD, V - VBlank
  uint8_t *p_IF; // Interrupt Flag (0xFF0F)
  uint8_t *p_IE; // Interrupt Enable (0xFFFF)

  uint8_t step(uint8_t opcode);
  void compute_current_opcode_groupings(uint8_t &x, uint8_t &y, uint8_t &z);
  void print_registers();

  void call(uint8_t arg1, uint8_t arg2);
  void push(uint16_t value);
  uint16_t pop();
  void panic(); // Custom method to handle our own implementation mistakes

  uint8_t *get_register(uint8_t i);
};

#endif