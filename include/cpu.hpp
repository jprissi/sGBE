#ifndef CPU_H
#define CPU_H

// #include <cstdint>
#include <stdint.h>

#include "memory.hpp"
#include <fstream>
// #include "opcodes.hpp"

class CPU
{
public:
  // uint8_t A; //, B, C, D, E, F;
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

  // uint16_t PC = 0x100; //256; // 0x100
  uint16_t PC = 0x0000; // Before boot

  uint8_t next_instruction_relative_pos;
  

  void set_breakpoint(uint16_t address);

private:
  int32_t total_cycles = 0;
  uint16_t breakpoint = 0xffff;
  void assertCPUInitialization();
public:

  void log(std::ofstream &file);
  bool step_by_step = false;
  bool interrupts_enabled = true;
  bool disable_interrupts_next = false; // Or use a counter instead

  void push(uint16_t value);
  uint16_t pop();

  uint8_t decode(uint8_t opcode);
  void extract_grouping(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z);
  void print_registers();

  uint8_t *get_register(uint8_t i);

  // void to_16bits(uint8_t in1, uint8_t in2, uint16_t &out);
  // void to_8bits(uint16_t in, uint8_t &out1, uint8_t &out2);

  void panic(); // Custom method to handle our own implementation mistakes

  uint8_t *registers[8] = {this->p_B, this->p_C, this->p_D, this->p_E, this->p_H, this->p_L, reinterpret_cast<uint8_t *>(&this->HL), this->p_A};
  // uint8_t *registers[8] = {this->p_B, this->p_C, this->p_D, this->p_E, this->p_H, this->p_L, this->m.get_pointer(this->HL), this->p_A};
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

  CPU(std::string cartridge_path);
};

#endif