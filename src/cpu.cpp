#include "cpu.hpp"
#include "opcodes.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>


void CPU::load16(uint8_t arg1, uint8_t arg2){

}

void CPU::nop(uint8_t a = NULL, uint8_t = NULL){};

void CPU::jump(uint8_t arg1, uint8_t arg2)
{
  // uint16_t address = arg2 << 8 | arg1; // (LS byte first)
  //Let's try the other way around
  uint16_t address = arg1 << 8 | arg2;
  this->PC = address - 3;
  std::cout << std::dec << "⇩" << std::endl;

}

CPU::CPU()
{
  // this->opcodes =
}

// void CPU::fetch(){

// }

void CPU::split_opcode(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z){
  x = opcode >> 6;
  y = (opcode >> 3) & 7;
  z = opcode & 7;
}

uint8_t CPU::decode(uint8_t opcode)
{
  // http://www.z80.info/decoding.htm

  std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)opcode;
  std::cout << std::dec << " (" << std::setw(3) << (int)opcode << ")\t";
  // << std::endl;

  // Find instruction groupings
  uint8_t x, y, z;
  this->split_opcode(opcode,x,y,z);

  const char *r[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
  uint8_t *r2[8] = {&this->B, &this->C, &this->D, &this->E,
                    &this->H, &this->L, reinterpret_cast<uint8_t *>(&this->HL), &this->A};
  const char *rp[4] = {"BC", "DE", "HL", "SP"};
  const char *alu[8] = {"ADD A", "ADC A", "SUB", "SBC A", "AND", "XOR", "OR", "CP"};

  // Everything we need to run instruction
  std::string mnemonic("");
  int num_args;
  arguments[0] = NULL;
  arguments[1] = NULL;
  instr fn;

  // Handle grouped instructions first and then use LUT if necessary (move
  // grouped instruction to their own 2D LUT later on)
  if (x == 0 && z == 6)
  {
    // 8 bit load immediate
    // cycles = 8;
    char *reg = (char *)r[y];

    // Then LD
    mnemonic.append("LD ");
    mnemonic.append(reg);
    mnemonic.append(",n");

    fn = &CPU::nop;
    num_args = 1;
  } 
  else if (x==0 && z==4) {
    char *reg = (char *)r[y];
    mnemonic.append("INC ");
    mnemonic.append(reg);

    fn = &CPU::nop;
    //cycles = 4
    num_args = 0;
  }
  else if (x==1 && (z!=6 || y!=6)){
    char *reg1 = (char *)r[y];
    char *reg2 = (char *)r[z];
    mnemonic.append("LD ");
    mnemonic.append(reg1);
    mnemonic.append(", ");
    mnemonic.append(reg2);

    fn = &CPU::nop;
    //cycles = 4
    num_args = 0;    

  }
  else if (x==2) {
    mnemonic.append(alu[y]);
    mnemonic.append(r[z]);

    fn = &CPU::nop;
    //cycles 4 or 8
    num_args = 1;
  }
  else if (x==3 && z==7) {
    // std::cout << std::hex << (int) x << "; " << (int)y << "; " << (int) z << std::endl;
    // RST n
    // 32 cycles
    mnemonic.append("RST n");
    // Push present address onto stakc
    //TODO
    // JMP to $0000 + n
    fn = &CPU::jump;
    num_args = 0;
    arguments[0] = 8*y;
    arguments[1] = 0;

  }
  else if (opcodes[opcode].mn == "UNK")
  {
    // Handle opcodes that are not implemented
    std::cout << std::endl;
    std::cerr << "Instruction not implemented" << std::endl;
    std::cout << std::hex << (int)x << "; " << (int)y << "; " << (int)z
              << std::endl;
    exit(1);
  }
  else
  {
    // Default opcode table
    mnemonic = (char *)opcodes[opcode].mn;
    num_args = opcodes[opcode].length - 1;
    fn = opcodes[opcode].func;
  }

  std::cout << mnemonic << "\t";

  if (num_args > 0)
  {
    std::cout << "args(" << num_args << "): ";
    for (int i = 1; i < num_args + 1; ++i)
    {
      // Starting from 1 to remove current opcode
      // PC + 1 (current PC?)
      arguments[i - 1] = (int)this->m.rom[this->PC + i];
      std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)arguments[i - 1] << " ";
    }
  }

  std::cout << std::dec << std::flush;
  std::cout << std::endl;

  // std::cout << std::hex << "===\ndebug\n"
  //           << this->PC << ";" << arguments[0] << ";" << arguments[1] << std::endl;

  (this->*fn)(arguments[0], arguments[1]);

  return num_args + 1;
}