#include "opcodes.hpp"
#include <iomanip>
#include <iostream>
#include <stdio.h>

uint8_t decode(unsigned char opcode) {
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

  std::string mnemonic("");
  int args;

  // Handle grouped instructions first and then use LUT if necessary (move
  // grouped instruction to their own 2D LUT later on)
  if (x == 0 && z == 6) {
    // 8 bit load immediate
    // cycles = 8;
    char *reg = (char*)r[y];

    // Then LD
    mnemonic.append("LD ") ;
    mnemonic.append(reg);
    mnemonic.append(",n");
    args = 1;

  } else if (opcodes[opcode].mn == "UNK") {
    // Handle opcodes that are not implemented
    std::cout << std::endl;
    std::cerr << "Instruction not implemented" << std::endl;
    std::cout << std::hex << (int)x << "; " << (int)y << "; " << (int)z
              << std::endl;
    exit(1);
  } else {
    // Default opcode table
    mnemonic = (char*)opcodes[opcode].mn;
    args = opcodes[opcode].length - 1;
  }

  std::cout << mnemonic << "\t";
  std::cout << std::endl;

  return args;
}

// int cycles = 0;
// void execute(unsigned char opcode) {
//   Z80_opcodes instruction = opcodes[opcode];
//   uint8_t num_arguments = instruction.length - 1;

//   char args[] = "aaa";
//   instruction.func();

//   cycles += instruction.cycles;
// }

void nop(){};