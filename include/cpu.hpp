#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <stdint.h>

#include "memory.hpp"

class CPU {

  uint8_t A, B, C, D, E, F;
  uint16_t HL;
  uint8_t H = HL;
  uint8_t L = HL + 1;
  uint8_t flag;

  uint16_t SP;
  

  

public:
  uint8_t decode(uint8_t opcode);
  Memory m;
  uint16_t PC = 256; // 0x100
  // CPU();
};

#endif