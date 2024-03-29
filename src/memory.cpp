#include <stdio.h>
#include <stdint.h>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <bitset>

#include "memory.hpp"
#include "cpu.hpp"
#include "opcodes.hpp"

#define STAT 0xFF41

// http://www.z80.info/decoding.htm

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
  // Lots of additional subtleties in here (should block ppu, ...)

  uint16_t source = this->read(DMA) << 8;
  uint16_t destination = 0xFE00;
  std::size_t size = 0x9f;

  uint8_t *src_address = this->get_pointer(source);
  uint8_t *dst_address = this->get_pointer(destination);

  std::memcpy(dst_address, src_address, size);
}

void MemoryController::write(uint16_t address, uint8_t value)
{
  if (address >= 0xE000 && address < 0xFE00)
  {
    // Echo of internal RAM
    address += 0xC000 - 0xE000;
  }
  else if (address >= 0xFE00 && address < 0xFE9F)
  {
    return write_oam(address, value);
  }
  else if (address >= 0x8000 && address < 0x9FFF)
  {
    return write_vram(address, value);
  }
  else if (address < 0x8000)
  {
    std::cout << std::endl
              << "Skipping write to ROM :  " << std::hex << (int)address << std::endl;
    return;
  }
  else if (address == DMA)
  {
    // Initiating DMA transfer (160 machine cycles: 1.4 lines: 640 dots)
    std::cout << "Requesting DMA transfer" << std::endl;
    this->rom[address] = value; // Write DMA address before calling transfer
    this->DMA_transfer();
    return;
  }
  else if (address == 0xFF50)
  {
    std::cout << std::endl
              << "Disabling boot rom" << std::endl;
    boot_rom_enabled = false;
  }
  else if (address == STAT)
  {
    // std::cout << "reading STAT (0xFF41)" << std::endl;
  }

  this->rom[address] = value;
};

uint8_t MemoryController::read(uint16_t address, bool is_ppu)
{
  if (address >= 0xE000 && address < 0xFE00)
  {
    // Echo of internal RAM
    address += 0xC000 - 0xE000;
  }
  else if (address >= 0x8000 && address < 0x9FFF)
  {
    read_vram(address, is_ppu);
  }
  else if (address >= 0xFE00 && address < 0xFE9F)
  {
    read_oam(address);
  }
  else if (address >= 0xFEA0 && address < 0xFEFF)
  {
    // Not usable
    std::cout << address << std::endl;
    exit(1);
  }

  // 0xFF50 = 1 => boot ROM disabled
  if (boot_rom_enabled && address < 0x0100)
  {
    return this->boot_rom[address];
  }

  if (address >= 0xFF00 && address < 0xFF80)
  {
    return read_IO_register(address);
  }

  return this->rom[address];
}

uint8_t MemoryController::read_vram(uint16_t address, bool is_ppu)
{
  // If LCDC disabled, immediate and full access to VRAM, OAM, etc
  bool ppu_enabled = (rom[LCDC] >> 7) & 1;
  uint8_t ppu_status = (rom[STAT] & 3);
  if (ppu_status == 3 && ppu_enabled && !is_ppu) // VRAM accessible during mode 0-2
  {
    return 0xFF; // undefined data
  }
  return this->rom[address];
}

uint8_t MemoryController::read_oam(uint16_t address)
{
  // OAM accessible during mode 0-1 else do nothing
  bool ppu_enabled = (rom[LCDC] >> 7) & 1;
  uint8_t ppu_status = (rom[STAT] & 0x03);
  if (ppu_status >= 2 && ppu_enabled)
  {
    // Return undefined data
    return 0xFF;
  }
}

void MemoryController::write_vram(uint16_t address, uint8_t value)
{
  // VRAM accessible during mode 0-2 else do nothing
  bool ppu_enabled = (rom[LCDC] >> 7) & 1;
  uint8_t ppu_status = (rom[STAT] & 0x03);
  if (ppu_status == 3 && ppu_enabled)
  {
    // Do nothing
    return;
  }
  this->rom[address] = value;
}
void MemoryController::write_oam(uint16_t address, uint8_t value)
{
  // OAM accessible during mode 0-1 else do nothing
  bool ppu_enabled = (rom[LCDC] >> 7) & 1;
  uint8_t ppu_status = (rom[STAT] & 0x03);
  if (ppu_status >= 2 && ppu_enabled)
  {
    return;
  }
  this->rom[address] = value;
}

uint8_t MemoryController::read_IO_register(uint16_t address)
{
  bool is_address_invalid = (address == 0xFF03 ||
                             (address >= 0xFF08 && address < 0xFF0F) ||
                             (address >= 0xFF27 && address < 0xFF30) ||
                             (address >= 0xFF4C && address < 0xFF50) ||
                             (address >= 0xFF56 && address < 0xFF68) ||
                             (address >= 0xFF6C && address < 0xFF70));

  // Handle known cases
  if (address == 0xFF4D)
  {
    return 0xFF;
  }
  // Fallback
  if (is_address_invalid)
  {
    std::cout.clear();
    std::cout << "Unexpected I/O read: " << (int)address << std::endl;
    exit(1);
  }

  switch (address)
  {
  case 0xFF00:
    return read_joypad();
  case 0xFF4D:
    // Undocumented
    return 0xFF;
    // default:
    //   std::cout.clear();
    //   std::cout << "Unknown I/O: " << (int)address << std::endl;
    //   exit(1);
  }

  return this->rom[address];
}

uint8_t MemoryController::read_joypad()
{
  // std::cout.clear();
  // std::cout << "===\n reading joypad" << std::endl;
  uint8_t value = (rom[0xFF00] & 0xF0);
  // std::cout << (int)value << std::endl;
  // std::cout << std::bitset<8>(joypad_registers.input_a) << std::endl;
  // std::cout << std::bitset<8>(joypad_registers.input_b) << std::endl;
  if (!((value >> 5) & 1))
  {
    value |= (joypad_registers.input_a & 0x0F);
  }
  else if (!((value >> 6) & 1))
  {
    value |= (joypad_registers.input_b & 0x0F);
  }
  else
  {
    value |= 0x0F;
  }
  // std::cout << "value: " << std::bitset<8>(value) << std::endl;
  // std::cin.ignore();
  // std::cout.setstate(std::ios_base::failbit);
  return value;
}

void MemoryController::set_joypad(uint8_t a, uint8_t b)
{
  joypad_registers.input_a = a;
  joypad_registers.input_b = b;
}

// uint16_t MemoryController::read16(uint16_t address)
// { // Not working
//   std::cout << "read16 not working" << std::endl;
//   exit(1);
//   uint16_t value = this->rom[address] + (this->rom[address - 1] << 8);
//   return value;
// }

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

  // Create input file stream
  std::ifstream input(file_path, std::ios::binary);

  if (!input.is_open())
  {
    std::cerr << "Failed to open file :" << file_path << std::endl;
    exit(1);
  }

  input.seekg(0, std::ios::end);
  std::size_t size = input.tellg();
  std::filebuf *pbuf = input.rdbuf();
  pbuf->pubseekpos(0, input.in);
  char *buffer = (char *)(target);
  pbuf->sgetn(buffer, size);

  std::cout << "ROM size: " << size << " bytes" << std::endl;

  input.close();
}

uint8_t *MemoryController::get_pointer(uint16_t address)
{
  return &this->rom[address];
};

void MemoryController::enable_bootrom()
{
  boot_rom_enabled = true;
}

void MemoryController::disable_bootrom()
{
  boot_rom_enabled = false;
}