
#include <stdio.h>
#include <cstddef>
// #include <cstdint>
#include <stdint.h>
// #include <stdio.h>
#include <cstring>
// #include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <bitset>

#include "memory.hpp"
#include "cpu.hpp"
#include "opcodes.hpp"

// http://www.z80.info/decoding.htm

#define ROM_OFFSET 0x00 // Cartridge starts at address 0

std::string boot_rom_path = "../rom/dmg_boot.bin";

MemoryController::MemoryController()
{
  bool is_boot = true;
  this->copy_file_to_memory(boot_rom_path, is_boot);
  this->rom[0xFF00] = 0xFF; // Joypad input defaults to 0xFF at startup
}

void MemoryController::DMA_transfer()
{
  // https://gbdev.io/pandocs/OAM_DMA_Transfer.html
  // 160 machine cycles
  // Lots of subtleties here (should block ppu, ...)
  uint16_t source = this->read(DMA) << 8;
  uint16_t destination = 0xFE00;
  uint8_t size = 0x9f;

  uint8_t *src_address = this->get_pointer(source);
  uint8_t *dst_address = this->get_pointer(destination);

  std::memcpy(dst_address, src_address, size);
}

void MemoryController::write(uint16_t address, uint8_t value)
{
  // TODO: Check if memory is locked
  if (address >= 0xE000 and address < 0xFE00)
  {
    // Echo of internal RAM
    address += 0xC000 - 0xE000;
  }

  if (address < 0x8000)
  {
    std::cout << std::endl
              << "Trying to write to ROM :  " << std::hex << (int)address << std::endl;
    // exit(1);
  }
  else if (address == DMA)
  {
    // Initiating DMA transfer (160 machine cycles: 1.4 lines: 640 dots)
    std::cout << "Requesting DMA transfer" << std::endl;
    // exit(1);
    this->DMA_transfer();

    // memcpy(value*0x100, 0xFE00, 0xFE9F-0xFE00);
  }
  else
  {
    this->rom[address] = value;
  }
  if (address == 0x691e)
  {
    std::cout << "writing " << std::hex << (int)value << " to 0x691e" << std::endl;
    std::cin.ignore();
  }
  // if (address == 0xdf7e){
  //   std::cout << "writing " << std::hex << (int)value << " to 0x691e" << std::endl;
  //   std::cin.ignore();
  // }
  if (address == 0xFF50)
  {
    std::cout << std::endl
              << "Disabling boot rom" << std::endl;
    boot_rom_enabled = false;
  }

  // if (address == TAC)
  // {
  //   std::cout << "writing to TAC" << std::endl;
  //   // std::cin.ignore();
  // }
};

// void MemoryController::write(uint16_t address, uint16_t value)
// {
//   this->rom[address] = value;
// }

uint8_t MemoryController::read(uint16_t address)
{
  if (address >= 0xE000 and address < 0xFE00)
  {
    // Echo of internal RAM
    address += 0xC000 - 0xE000;
  }

  // 0xFF50 = 1 => boot ROM disabled
  if (boot_rom_enabled && address < 0x0100)
  {
    return this->boot_rom[address];
  }

  if (address == 0xFF00)
  {
    // No button pressed
    return 0xFF;
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
