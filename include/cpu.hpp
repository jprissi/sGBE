#ifndef CPU_H
#define CPU_H

// #include <cstdint>
#include <stdint.h>

#include "memory.hpp"
// #include "opcodes.hpp"

class CPU
{
public:
  uint8_t A; //, B, C, D, E, F;
  uint16_t BC, DE, HL;
  uint8_t *p_B = (uint8_t *)&BC;
  uint8_t *p_C = p_B + 1;
  uint8_t *p_D = (uint8_t *)&DE;
  uint8_t *p_E = p_D + 1;
  uint8_t *p_H = (uint8_t *)&HL;
  uint8_t *p_L = p_H + 1;

  uint8_t flags; // [Z, N, H, C, 0, 0, 0, 0] 8 bits

  uint16_t SP = 0xFFE;
  uint16_t PC = 0x100; //256; // 0x100

private:
  int32_t total_cycles = 0;
  
public:
  bool interrupts_enabled = true;
  bool disable_interrupts_next = false; // Or use a counter instead

  void push(uint16_t value);
  uint16_t pop();

  uint8_t decode(uint8_t opcode);
  void extract_grouping(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z);

  void to_16bits(uint8_t in1, uint8_t in2, uint16_t &out);
  void to_8bits(uint16_t in, uint8_t &out1, uint8_t &out2);

  void crash();

  uint8_t *registers[8] = {this->p_B, this->p_C, this->p_D, this->p_E, this->p_H, this->p_L, reinterpret_cast<uint8_t *>(&this->HL), &this->A};
  uint16_t *registers16[4] = {&this->BC,
                              &this->DE,
                              &this->HL,
                              &this->SP
                              };
  Memory m;

  // CPU();
};

#endif