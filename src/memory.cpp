#include <memory.hpp>
#include <stdio.h>
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <bitset>

#include "cpu.hpp"
#include "opcodes.hpp"

// http://www.z80.info/decoding.htm

#define ROM_OFFSET 0x00 // Cartridge starts at address 0

std::string boot_rom_path = "../rom/dmg_boot.bin";

MemoryController::MemoryController()
{
  bool is_boot = true;
  this->copy_file_to_memory(boot_rom_path, is_boot);
}

void MemoryController::write(uint16_t address, uint8_t value)
{
  // TODO: Check if memory is locked
  this->rom[address] = value;

  if (address == 0xFF50)
  {
    std::cout << std::endl
              << "Disabling boot rom" << std::endl;
    boot_rom_enabled = false;
  }
};

void MemoryController::write(uint16_t address, uint16_t value)
{
  this->rom[address] = value;
}

uint8_t MemoryController::read(uint16_t address)
{
  // 0xFF50 = 1 => boot ROM disabled
  if (boot_rom_enabled && address < 0x0100)
  {
    return this->boot_rom[address];
  }
  return this->rom[address];
}

uint16_t MemoryController::read16(uint16_t address)
{ // Not working
  std::cout << "read16 not working" << std::endl;
  exit(1);
  uint16_t value = this->rom[address] + (this->rom[address - 1] << 8);
  return value;
}

void MemoryController::load_cartridge(std::string &file_path)
{
  this->copy_file_to_memory(file_path);
}

void MemoryController::copy_file_to_memory(std::string &file_path, bool is_boot)
{
  // Load ROM to memory

  uint8_t *target = this->rom;
  if (is_boot)
  {
    target = this->boot_rom;
  }

  uint8_t *p_rom_buffer;

  // Create input file stream
  std::ifstream input(file_path, std::ios::binary);

  if (!input.is_open())
  {
    std::cerr << "Failed to open file\n";
    exit(1);
  }

  input.seekg(0, std::ios::end);
  std::size_t size = input.tellg();
  std::cout << "ROM size: " << size << " bytes" << std::endl;
  std::filebuf *pbuf = input.rdbuf();
  pbuf->pubseekpos(0, input.in);
  char *buffer = (char *)(target + ROM_OFFSET);
  pbuf->sgetn(buffer, size);

  input.close();
}

uint8_t *MemoryController::get_pointer(uint16_t address)
{
  return &this->rom[address];
};
