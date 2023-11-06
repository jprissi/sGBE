#include "cpu.hpp"
#include "opcodes.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>


void CPU::push(uint16_t value){
  this->SP--;
  this->m.rom[this->SP] = value;
}

void CPU::to_16bits(uint8_t in1, uint8_t in2, uint16_t &out){
  out = (in1 << 8) | in2;
}
void CPU::to_8bits(uint16_t in, uint8_t &out1, uint8_t &out2){
  out1 = (in & (0b1111 << 4)) >> 4;
  out2 = in & 0b1111;
}

void CPU::extract_grouping(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z){
  // Find instruction groupings
  // uint8_t x, y, z;
  x = opcode >> 6;
  y = (opcode >> 3) & 7;
  z = opcode & 7;

  // const char *r[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
  // 
}

uint8_t CPU::decode(uint8_t opcode)
{
  // http://www.z80.info/decoding.htm

  std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)opcode;
  std::cout << std::dec << " (" << std::setw(3) << (int)opcode << ")\t";

  std::string mnemonic("");
  int num_args, cycles = 0;

  if (sizeof(opcodes)/sizeof(*opcodes) - 1 < opcode){
      std::cout << std::endl;
      std::cout << "Opcode ";
      std::cout << std::hex << (int)opcode;
      std::cout << std::dec << " not in opcodes!" << std::endl;
      exit(1);
  }
    
  opcodes_s current_op = opcodes[opcode];

  mnemonic = (char *)current_op.mn;
  num_args = current_op.length - 1;
  cycles = current_op.cycles;
  auto func = current_op.func;

  // std::cout << std::hex << (int)opcode << std::endl;
  std::cout << mnemonic << "\t";

  uint8_t args[2] = {NULL, NULL};
  // uint8_t arg2 = NULL;

  if (num_args > 0)
  {
    std::cout << "args: ";
    for (int i = 1; i < num_args+1; ++i)
    {
      // Starting from 1 to remove current opcode
      // PC + 1 (current PC?)
      std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)this->m.rom[this->PC + i] << " ";
      args[i-1] = (uint8_t)this->m.rom[this->PC + i];
    }
  }

  // Run instruction
  (*func)(*this, args[0], args[1]);
  this->total_cycles += cycles;
    

  this->PC += 1 + num_args;

  std::cout << std::dec << std::flush;
  std::cout << std::endl;

  return num_args;
}