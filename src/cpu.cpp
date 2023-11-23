#include "cpu.hpp"
#include "opcodes.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include <bitset>
#include <assert.h>

void CPU::default_init()
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
}

CPU::CPU(std::string cartridge_path, bool debug_implementation)
{

  this->p_IE = this->m.get_pointer(InterruptEnableAddress);
  this->p_IF = this->m.get_pointer(InterruptFlagAddress);

  // Skip boot rom
  if (debug_implementation)
  {
    this->default_init();
    this->assert_init();
    this->m.write((uint16_t)0xFF44, (uint8_t)0x90);
    this->m.boot_rom_enabled = false;
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

void CPU::call(uint8_t arg1, uint8_t arg2)
{

  uint16_t PC_next = this->PC + this->next_instruction_relative_pos;
  this->push(PC_next);

  this->PC = (uint16_t)(arg2 << 8 | arg1) - this->next_instruction_relative_pos;
}

void CPU::push(uint16_t value)
{

  // uint8_t arg1 = (value & 0xFF00) >> 8;
  // uint8_t arg2 = (value & 0x00FF);

  this->SP--;
  this->m.write(this->SP, value >> 8);
  this->SP--;
  this->m.write(this->SP, value & 0xFF);

  // uint8_t *p_rom = this->m.get_pointer(this->SP);
  // *p_rom = arg1;
  // *(p_rom-1) = arg2;
  // this->SP-=2;
}

uint16_t CPU::pop()
{

  // uint8_t *p_rom = this->m.get_pointer(this->SP);
  uint8_t arg1 = this->m.read(this->SP);
  (this->SP)++;
  uint8_t arg2 = this->m.read(this->SP);
  (this->SP)++;

  uint16_t value = (arg2 << 8) | arg1;

  return value;
}

void CPU::enable_interrupts()
{
  this->interrupts_enabled = true;
};

void CPU::disable_interrupts()
{
  this->interrupts_enabled = false;
};

void CPU::handle_interrupts()
{
  // Interrupt priority : 0 (VBlank) -> 4 (Joypad)
  bool interrupt_enabled, interrupt_requested;
  // std::cout << std::endl << "interrupts:" << std::endl;
  for (int i = 0; i < 5; ++i)
  {
    interrupt_enabled = ((this->m.read(InterruptEnableAddress) >> i) & 1);
    interrupt_requested = ((this->m.read(InterruptFlagAddress) >> i) & 1);

    // std::cout << std::hex << (int)interrupt_enabled << "; " << (int)interrupt_requested << std::endl;
    if ((interrupt_enabled && interrupt_requested))
    // Apparently halt does not need IME to be set
    {
      std::cout << "Interrupt!" << std::endl;
      // this->step_by_step = true;
      if (this->halt)
      {
        // this->step_by_step = true;
        this->halt = false; // Just in case
        if (this->skip_interrupts)
        {
          this->skip_interrupts = false;
          return;
        }
        // this->PC ++;
      }
      // this->step_by_step = true;
      this->interrupts_enabled = false;                              // Disabled IME
      uint8_t flag = this->m.read(InterruptFlagAddress) & ~(1 << i); // Reset interrupt flag
      this->m.write(InterruptFlagAddress, flag);

      // NOP(*this, 0, 0);
      // NOP(*this, 0, 0);     // Two M-cycles pass while nothing happens
      // this->push(this->PC); // 2 M-cycles
      // std::cout << std::hex << (int)((i * 8) + 0x40) << std::endl;

      // std::cout << "mem[0x40] = " << std::hex << (int)this->m.read(0x40) << std::endl;
      // this->step_by_step = true;

      this->PC -= this->next_instruction_relative_pos; // quick fix because of call incrementing pc and interrupts being called after pc is updated
      this->call((uint8_t)((i * 8) + 0x40) + this->next_instruction_relative_pos, (uint8_t)0x00);

      // this->push(this->PC);
      // this->PC = (uint16_t)(arg2 << 8 | arg1) - this->next_instruction_relative_pos;
    }
  }
}

void CPU::handle_timer(uint8_t cycles)
{
  cycles_count_div += cycles;
  uint8_t val = cycles_count_div * 16.384e3 / CPU_FREQ;
  if (val > 0)
  {
    cycles_count_div -= val * CPU_FREQ / 16.384e3;
  }

  uint8_t div = this->m.read(DIV);
  this->m.write(DIV, div + val); // Loops at FF00 (int16_t is enough));

  uint8_t tac = this->m.read(TAC);
  bool tima_enabled = (tac >> 2) & 1;

  if (tima_enabled)
  {

    uint16_t clock_divider;
    switch (tac & 3)
    {
    case 0:
      clock_divider = 1024;
      break;
    case 1:
      clock_divider = 16;
      break;
    case 2:
      clock_divider = 64;
      break;
    case 3:
      clock_divider = 256;
      break;
    };

    uint8_t tima = this->m.read(TIMA);
    cycles_count += cycles;
    uint8_t val = cycles_count * (CPU_FREQ / clock_divider) / CPU_FREQ;
    if (val > 0)
    {
      cycles_count -= val / ((CPU_FREQ / clock_divider) / CPU_FREQ);
    }

    // std::cout << (int)clock_divider << std::endl;
    // std::cout << std::hex << (int)cycles_count << std::endl;
    // std::cout << std::hex << (int)val << std::endl;
    // std::cout << "tima (before): " << std::hex << (int)tima << std::endl;
    // this->step_by_step = true;

    if ((uint16_t)tima + (uint16_t)val > 0xFF)
    {
      std::cout << "Timer Interrupt" << std::endl;
      // this->step_by_step = true;
      // When timer overflows
      this->request_interrupt(INT_TIMER);
      uint8_t tma = this->m.read(TMA);
      tima = tma;
      this->m.write(TIMA, tima);
    }
    else
    {
      tima += val;
      this->m.write(TIMA, tima);
    }

    // std::cout << "tima (after): " << std::hex << (int)tima << std::endl;
  }
};

void CPU::request_interrupt(uint8_t interrupt)
{
  (*this->p_IF) |= interrupt;
}

void CPU::compute_current_opcode_groupings(uint8_t &x, uint8_t &y, uint8_t &z)
{
  uint8_t opcode = this->current_opcode;
  this->compute_opcode_groupings(opcode, x, y, z);
}

void CPU::compute_opcode_groupings(uint8_t &opcode, uint8_t &x, uint8_t &y, uint8_t &z)
{
  // Compute instruction groupings
  // uint8_t x, y, z;
  x = opcode >> 6;
  y = (opcode >> 3) & 7;
  z = opcode & 7;
}

void CPU::assert_init()
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

uint8_t CPU::step(uint8_t opcode, bool log_to_file)
{
  // http://www.z80.info/decoding.htm
  if (!this->halt)
  {
    if (log_to_file)
      this->log(this->log_file);
  }
  // Everything we need to run instructions
  std::string mnemonic("");
  int num_args, cycles = 0;
  bool prefixed = false;
  uint8_t prefix;

  if (sizeof(opcodes) / sizeof(*opcodes) - 1 < opcode)
  {
    std::cout << std::endl
              << "Opcode " << std::hex << (int)opcode << " not in opcodes!" << std::endl;
    this->panic();
  }

  this->current_opcode = opcode;
  opcodes_s current_op = opcodes[opcode];

  if (current_op.mn == "PREFIX")
  {
    prefixed = true;
    prefix = opcode; // should be CB
    opcode = this->m.read(this->PC + 1);
    this->current_opcode = opcode;
    
    current_op = prefixed_opcodes[opcode];
    
  }

  mnemonic = (char *)current_op.mn;
  num_args = current_op.length - 1;
  cycles = current_op.cycles;
  // void *(func)(void *) = current_op.func;
  void (*func)(CPU &cpu, uint8_t arg1, uint8_t arg2) = current_op.func;
  // auto func = current_op.func;

  uint8_t args[2] = {0, 0};

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

  // How many bytes forward
  next_instruction_relative_pos = 1 + num_args;

  if (this->PC == 0x100)
  {
    assert_init();
  }

  if (this->PC == breakpoint)
  {
    std::cout << std::endl
              << "breakpoint (step-by-step, press key to go forward)" << std::endl;
    print_registers();
    step_by_step = true;
    // exit(0);
  }

  if (this->enable_interrupts_next)
  {
    std::cout << "enabling interrupts";
    // this->step_by_step=true;
    this->enable_interrupts(); // Won't prevent execution of current instruction
    this->enable_interrupts_next = false;
  }
  if (this->disable_interrupts_next)
  {
    std::cout << "disabling interrupts";
    this->disable_interrupts();
    this->disable_interrupts_next = false;
  }

  if (!this->halt)
  {
    std::cout << std::endl;
    std::cout << std::hex << "[0x" << std::setw(4) << this->PC << "]\t";
    std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)opcode << " ";
    if (num_args > 0)
    {
      std::cout << "args: ";
      for (int i = 1; i < num_args + 1; ++i)
      {
        // Starting from 1 to remove current opcode
        int offset = prefixed; // 0 or 1;
        std::cout << std::setw(2) << std::hex << std::setfill('0') << (int)this->m.read(this->PC + i) + offset << " ";
      }
    }
    else
    {
      std::cout << " \t";
    }

    std::cout << "\t" << current_op.label << " ";

    // Run instruction
    (*func)(*this, args[0], args[1]);

    this->total_cycles += cycles;

    if (increment_pc_next)
    {
      this->PC += next_instruction_relative_pos;
    }
    else
    {
      increment_pc_next = true; // Halt bug
    }

    /* Check for loops*/
    uint8_t next_opcode = int(this->m.read(this->PC));
    if (opcode == next_opcode)
      this->timeout--;
    else
      timeout = 30;
    
  }
  else
  {
    cycles = 4; // If we stop the clock, timer won't work
    this->total_cycles += cycles;
  }

  this->handle_timer(cycles);

  // /* Check for interrupts */
  // if (skip_interrupts)
  // {
  //   skip_interrupts = false;
  // }
  // else
  // {

  if (this->interrupts_enabled || this->halt)
  {
    // if (this->skip_interrupts){
    //   this->skip_interrupts = false;
    // }
    this->handle_interrupts();
    // } else if (this->halt){
    //   this->halt = false;
  }
  // }

  return cycles;
}

void CPU::log(std::ofstream &log_file)
{
  // Log used with gameboy-doctor
  // A:00 F:11 B:22 C:33 D:44 E:55 H:66 L:77 SP:8888 PC:9999 PCMEM:AA,BB,CC,DD
  log_file << std::hex;
  log_file << "A:" << std::setw(2) << std::setfill('0') << std::uppercase << (int)*this->p_A << " ";
  log_file << "F:" << std::setw(2) << std::setfill('0') << (int)*this->flags << " ";
  log_file << "B:" << std::setw(2) << std::setfill('0') << (int)*this->p_B << " ";
  log_file << "C:" << std::setw(2) << std::setfill('0') << (int)*this->p_C << " ";
  log_file << "D:" << std::setw(2) << std::setfill('0') << (int)*this->p_D << " ";
  log_file << "E:" << std::setw(2) << std::setfill('0') << (int)*this->p_E << " ";
  log_file << "H:" << std::setw(2) << std::setfill('0') << (int)*this->p_H << " ";
  log_file << "L:" << std::setw(2) << std::setfill('0') << (int)*this->p_L << " ";
  log_file << "SP:" << std::setw(4) << std::setfill('0') << (int)this->SP << " ";
  log_file << "PC:" << std::setw(4) << std::setfill('0') << (int)this->PC << " ";
  log_file << "PCMEM:" << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 1) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 2) << ",";
  log_file << std::setw(2) << std::setfill('0') << (int)this->m.read(this->PC + 3) << "\n";
}

void CPU::print_registers()
{
  std::cout << std::hex;
  std::cout << "Interrupts enabled: " << std::setw(1) << (int)(this->interrupts_enabled) << std::endl;
  std::cout << "IE = 0x" << std::setw(2) << (int)(*this->p_IE) << " (" << std::bitset<8>(*this->p_IE) << ")" << std::endl;
  std::cout << "IF = 0x" << std::setw(2) << (int)(*this->p_IF) << " (" << std::bitset<8>(*this->p_IF) << ")" << std::endl;

  std::cout << "PC = 0x" << std::setw(4) << (int)(this->PC) << std::endl;
  std::cout << "flags = 0x" << (int)(*this->flags) << "; " << std::bitset<8>(*this->flags) << "; boot rom enabled: " << (int)this->m.boot_rom_enabled << std::endl;
  std::cout << "A = 0x" << std::setw(2) << (int)(*this->p_A) << " (" << std::bitset<8>(*this->p_A) << ")" << std::endl;
  std::cout << "B = 0x" << std::setw(2) << (int)(*this->p_B) << " (" << std::bitset<8>(*this->p_B) << ")" << std::endl;
  std::cout << "C = 0x" << std::setw(2) << (int)(*this->p_C) << " (" << std::bitset<8>(*this->p_C) << ")" << std::endl;
  std::cout << "D = 0x" << std::setw(2) << (int)(*this->p_D) << " (" << std::bitset<8>(*this->p_D) << ")" << std::endl;
  std::cout << "E = 0x" << std::setw(2) << (int)(*this->p_E) << " (" << std::bitset<8>(*this->p_E) << ")" << std::endl;
  std::cout << "H = 0x" << std::setw(2) << (int)(*this->p_H) << std::endl;
  std::cout << "L = 0x" << std::setw(2) << (int)(*this->p_L) << std::endl;
  std::cout << "DE = 0x" << std::setw(4) << (int)(this->DE) << std::endl;
  std::cout << "[DE] = 0x" << std::setw(2) << (int)(this->m.read(this->DE)) << std::endl;
  std::cout << "HL = 0x" << std::setw(4) << (int)(this->HL) << std::endl;
  std::cout << "[HL] = 0x" << std::setw(4) << (int)(this->m.read(this->HL)) << std::endl;
  std::cout << "SP = 0x" << std::setw(4) << (int)(this->SP) << std::endl;
  std::cout << "[SP], [SP+1] ... = 0x" << std::setw(2) << (int)(this->m.read(this->SP)) << " 0x" << (int)(this->m.read(this->SP + 1)) << " 0x" << (int)(this->m.read(this->SP + 2)) << " 0x" << (int)(this->m.read(this->SP + 3)) << std::endl;
  std::cout << std::endl
            << (int)this->m.read(0xdf7e);
  std::cout << std::dec;
}