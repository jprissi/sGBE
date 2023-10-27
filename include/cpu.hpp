#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <stdint.h>

#include "memory.hpp"
#include "opcodes.hpp"

enum OPCODE_STRUCTURE
{
  O,
  OO,
  ON,
  ONN,
  JN,
  OON,
  OOd,
  OOdO,
  OOdN,
  OONN,
  II
};


uint8_t decode(unsigned char opcode);

class CPU
{

  typedef void (CPU::*instr)(uint8_t, uint8_t);

  struct Z80_opcodes
  { // The opcode value is implicit in the array index
    unsigned int length;
    OPCODE_STRUCTURE s;
    const char *mn; // Instruction mnemonic, not including data
    uint8_t cycles;
    // void (CPU::*func)(uint8_t, uint8_t);
    instr func;
  };

  

  uint8_t A, B, C, D, E, F;
  uint16_t HL;
  uint8_t H = HL;
  uint8_t L = HL + 1;
  uint8_t flag;

  uint16_t SP;
  uint8_t arguments[2];
  // void nop(uint8_t a, uint8_t b);

public:
  // void CPU::fetch();

  uint8_t decode(uint8_t opcode);
  void split_opcode(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z);
  
  Memory m;
  uint16_t PC = 256; // 0x100

  // Instructions
  void nop(uint8_t arg1, uint8_t arg2);
  void jump(uint8_t arg1, uint8_t arg2);
  void load16(uint8_t arg1, uint8_t arg2);

  Z80_opcodes opcodes[256] = {
      {1, O, "NOP", 4, &CPU::nop}, // 0x00
      {3, ONN, "LD   BC, $%02x%02x"}, // 0x01
      {1, O, "LD   (BC), A"},         // 0x02
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // Ok
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (10)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // Ok
      {1, O, "UNK"},
      {1, O, "UNK"},
      {3, ONN, "LD DE, nn", 12, &CPU::load16},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (20)
      {1, O, "UNK"},
      {1, O, "UNK"}, // Ok
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (30) //Ok
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (40)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (50)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (60)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (70)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (80)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (90)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "LD E,A", 4},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (100)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (110)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (120)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (130)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "ADD A,A", 4},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (140)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (150)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (160)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (170)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {2, ON, "XOR n (A)", 4, &CPU::nop}, // AF (175)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (180)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (190)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {3, ONN, "JP nn", 12, &CPU::jump}, // 0xC3 (195)
      {3, ONN, "LD (C),A", 8},           // 0xE2 (226)
      {3, ONN, "LD A,(C)", 8},           // 0xF2 (242)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (200)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (210)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (220)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "POP HL"}, // E1
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (230)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (240)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // (250)
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"},
      {1, O, "UNK"}, // {1, ON, "RST n", 32, (instr)&CPU::nop}, // RST OK
      
  };

  CPU();

};

#endif