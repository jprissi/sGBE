#include "cpu.hpp"
#include "opcodes.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
// #include <sys/types.h>
#include <bitset>
#include <assert.h>

CPU::CPU(std::string cartridge_path)
{
  // Skip boot rom
  if (false)
  {
    this->SP = 0x100;
    *this->p_A = 0x01;
    *this->flags = 0xB0;
    *this->p_B = 0x00;
    *this->p_C = 0x13;
    *this->p_D = 0x00;
    *this->p_E = 0xd8;
    *this->p_H = 0x01;
    *this->p_L = 0x4d;
    this->SP = 0xfffe;
    this->PC = 0x0100;
    this->m.boot_rom_enabled = false;
    this->assertCPUInitialization();
  }
  else
  {
    this->m.boot_rom_enabled = true;
  }

  this->m.load_cartridge(cartridge_path);

  
}

uint8_t *CPU::get_register(uint8_t i)
{
  if (i == 6)
  {
    // retrieve (HL) instead of HL
    return this->m.get_pointer(this->HL);
  }
  uint8_t *p_reg = this->registers[i];
  return p_reg;
}

void CPU::panic()
{
  std::cout << "\nPanic! Exiting...\n===\n";
  print_registers();
  exit(1);
}

void CPU::set_breakpoint(uint16_t address)
{
  breakpoint = address;
}

void CPU::push(uint16_t value)
{

  // std::cout << "\tpush: " << std::hex << (int)value;

  uint8_t arg1 = (value & 0xFF00) >> 8;
  uint8_t arg2 = (value & 0x00FF);

  this->m.write(this->SP, arg2);
  this->SP--;
  this->m.write(this->SP, arg1);
  this->SP--;
}

uint16_t CPU::pop()
{
  // (this->SP)++;
  uint8_t arg1 = this->m.read(this->SP + 1);
  // (this->SP)++;
  uint8_t arg2 = this->m.read(this->SP + 2);

  uint16_t value = (arg1 << 8) | arg2;
  this->SP += 2;

  // std::cout << "\tpop: " << std::hex << (int)value;

  return value;
}

// void CPU::to_16bits(uint8_t in1, uint8_t in2, uint16_t &out)
// {
//   out = (in1 << 8) | in2;
// }

// void CPU::to_8bits(uint16_t in, uint8_t &out1, uint8_t &out2)
// {
//   out1 = (in & (0b1111 << 4)) >> 4;
//   out2 = in & 0b1111;
// }

void CPU::print_registers()
{
  std::cout << "PC = 0x" << std::setw(4) << std::hex << (int)(this->PC) << std::endl;
  std::cout << "flags = 0x" << std::hex << (int)(*this->flags) << "; " << std::bitset<8>(*this->flags) << "; boot rom enabled: " << (int)this->m.boot_rom_enabled << std::endl;
  std::cout << "A = 0x" << std::setw(2) << std::hex << (int)(*this->p_A) << " (" << std::bitset<8>(*this->p_A) << ")" << std::endl;
  std::cout << "B = 0x" << std::setw(2) << std::hex << (int)(*this->p_B) << " (" << std::bitset<8>(*this->p_B) << ")" << std::endl;
  std::cout << "C = 0x" << std::setw(2) << std::hex << (int)(*this->p_C) << " (" << std::bitset<8>(*this->p_C) << ")" << std::endl;
  std::cout << "D = 0x" << std::setw(2) << std::hex << (int)(*this->p_D) << " (" << std::bitset<8>(*this->p_D) << ")" << std::endl;
  std::cout << "E = 0x" << std::setw(2) << std::hex << (int)(*this->p_E) << " (" << std::bitset<8>(*this->p_E) << ")" << std::endl;
  std::cout << "H = 0x" << std::setw(2) << std::hex << (int)(*this->p_H) << std::endl;
  std::cout << "L = 0x" << std::setw(2) << std::hex << (int)(*this->p_L) << std::endl;
  std::cout << "DE = 0x" << std::setw(4) << std::hex << (int)(this->DE) << std::endl;
  std::cout << "[DE] = 0x" << std::setw(2) << std::hex << (int)(this->m.read(this->DE)) << std::endl;
  std::cout << "HL = 0x" << std::setw(4) << std::hex << (int)(this->HL) << std::endl;
  std::cout << "[HL] = 0x" << std::setw(4) << std::hex << (int)(this->m.read(this->HL)) << std::endl;
  std::cout << "SP = 0x" << std::setw(4) << std::hex << (int)(this->SP) << std::endl;
  std::cout << "[SP+1] ... = 0x" << std::setw(2) << std::hex << (int)(this->m.read(this->SP + 1)) << " 0x" << (int)(this->m.read(this->SP + 2)) << " 0x" << (int)(this->m.read(this->SP + 3)) << std::endl;
}

void CPU::extract_grouping(uint8_t opcode, uint8_t &x, uint8_t &y, uint8_t &z)
{
  // Find instruction groupings
  // uint8_t x, y, z;
  x = opcode >> 6;
  y = (opcode >> 3) & 7;
  z = opcode & 7;

  // const char *r[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
  //
}

void CPU::assertCPUInitialization()
{

  *this->p_C = 0x13; // Should be set but unfortunately, wrong value TODO : fix

  assert(*this->p_A == 0x01);
  assert(*this->p_B == 0x00);
  assert(*this->p_C == 0x13);
  assert(*this->p_D == 0x00);
  assert(*this->p_E == 0xd8);
  assert(*this->p_H == 0x01);
  assert(*this->p_L == 0x4d);
  assert(this->SP == 0xfffe);
  // assert(this->PC == 0x0100);
}

uint8_t CPU::decode(uint8_t opcode)
{
  // http://www.z80.info/decoding.htm

  // std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)opcode;

  // Everything we need to run instruction
  std::string mnemonic("");
  int num_args, cycles = 0;

  if (sizeof(opcodes) / sizeof(*opcodes) - 1 < opcode)
  {
    std::cout << std::endl;
    std::cout << "Opcode ";
    std::cout << std::hex << (int)opcode;
    std::cout << std::dec << " not in opcodes!" << std::endl;
    exit(1);
  }

  opcodes_s current_op = opcodes[opcode];
  bool prefixed = false;
  uint8_t prefix;

  if (current_op.mn == "PREFIX")
  {
    prefixed = true;
    prefix = opcode; // should be CB
    opcode = this->m.read(this->PC + 1);
    current_op = prefixed_opcodes[opcode];
    // std::cout << " " << std::hex << (int)opcode;
  }

  // std::cout << " ";

  mnemonic = (char *)current_op.mn;
  num_args = current_op.length - 1;
  cycles = current_op.cycles;
  auto func = current_op.func;

  uint8_t args[2] = {NULL, NULL};

  if (num_args > 0)
  {
    // std::cout << "args: ";
    for (int i = 1; i < num_args + 1; ++i)
    {
      // Starting from 1 to remove current opcode
      int offset = prefixed; // 0 or 1;
      // std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)this->m.read(this->PC + i) + offset << " ";
      args[i - 1] = (uint8_t)this->m.read(this->PC + i + offset);
    }
  }
  else
  {
    // std::cout << "\t";
  }

  // How many bytes forward
  next_instruction_relative_pos = 1 + num_args;

  // std::cout << "\t" << current_op.label;

  if (this->PC == 0x100 && !this->m.boot_rom_enabled)
  {
    assertCPUInitialization();
  }

  if (this->PC == breakpoint) //&& !this->m.boot_rom_enabled)
  {
    std::cout << std::endl
              << "breakpoint (step-by-step, press key to go forward)" << std::endl;
    print_registers();
    step_by_step = true;
    // exit(0);
  }

  // Run instruction
  (*func)(*this, args[0], args[1]);
  this->total_cycles += cycles;

  this->PC += next_instruction_relative_pos;

  // std::cout << std::dec << std::endl;

  // return num_args;
  return cycles;
}

void CPU::log(std::ofstream &log_file)
{
  // A:00 F:11 B:22 C:33 D:44 E:55 H:66 L:77 SP:8888 PC:9999 PCMEM:AA,BB,CC,DD

  log_file << "A:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)*this->p_A << " ";
  log_file << "F:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_F << " ";
  log_file << "B:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_B << " ";
  log_file << "C:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_C << " ";
  log_file << "D:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_D << " ";
  log_file << "E:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_E << " ";
  log_file << "H:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_H << " ";
  log_file << "L:" << std::setw(2) << std::setfill('0') << std::hex << (int)*this->p_L << " ";
  log_file << "SP:" << std::setw(4) << std::setfill('0') << std::hex << (int)this->SP << " ";
  log_file << "PC:" << std::setw(4) << std::setfill('0') << std::hex << (int)this->PC << " ";
  log_file << "PCMEM:" << std::setw(2) << std::setfill('0') << std::hex << (int)this->m.read(this->PC) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 1) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 2) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 3) << "\n";
}