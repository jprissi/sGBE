#include "opcodes.hpp"
#include <stdio.h>
#include <iostream>

void decode(unsigned char opcode){
    // http://www.z80.info/decoding.htm
    unsigned char x, y, z;
    x = opcode >> 6 ;
    y = (opcode >> 3) & 7;
    z = opcode & 7;

    std::cout << std::hex << x << y << z << std::endl;
}


int cycles = 0;
void execute(unsigned char opcode) {
  Z80_opcodes instruction = opcodes[opcode];
  uint8_t num_arguments = instruction.length - 1;

  char args[] = "aaa";
  instruction.func();

  cycles += instruction.cycles;
}

void nop(){};