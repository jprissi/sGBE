#include "cpu.hpp"
#include "opcodes.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

// CPU::CPU(){};

uint8_t CPU::decode(uint8_t opcode)
{
  // http://www.z80.info/decoding.htm

  std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)opcode;
  std::cout << std::dec << " (" << std::setw(3) << (int)opcode << ")\t";
  // << std::endl;

  // Find instruction groupings
  uint8_t x, y, z;
  x = opcode >> 6;
  y = (opcode >> 3) & 7;
  z = opcode & 7;

  const char *r[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
  uint8_t *r2[8] = {&this->B, &this->C, &this->D, &this->E,
                    &this->H, &this->L, reinterpret_cast<uint8_t *>(&this->HL), &this->A};

  std::string mnemonic("");
  int args(0);

  if (sizeof(opcodes)/sizeof(*opcodes) - 1 < opcode){
      std::cout << std::endl;
      std::cout << "Opcode ";
      std::cout << std::hex << (int)opcode;
      std::cout << std::dec << " not in opcodes!" << std::endl;
      exit(1);
  }

  // if (opcodes[opcode].mn == "UNK")
  // {
  //   // Handle opcodes that are not implemented
  //   std::cout << std::endl;
  //   std::cerr << "Instruction not implemented" << std::endl;
  //   std::cout << std::hex << (int)x << "; " << (int)y << "; " << (int)z
  //             << std::endl;
  //   exit(1);
  // }

    // Default opcode table    
    // std::cout << opcode << std::endl << std::flush;
    
    mnemonic = (char *)opcodes[opcode].mn;
    args = opcodes[opcode].length - 1;
    
    auto func = opcodes[opcode].func;

    std::cout << mnemonic << "\t";

    (*func)(*this);

    



  if (args > 0)
  {
    std::cout << "args: ";
    for (int i = 1; i < args+1; ++i)
    {
      // Starting from 1 to remove current opcode
      // PC + 1 (current PC?)
      std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)this->m.rom[this->PC + 1 + i] << " ";
    }
  }
  std::cout << std::dec << std::flush;
  std::cout << std::endl;

  return args;
}