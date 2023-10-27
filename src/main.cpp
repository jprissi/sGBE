#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "cpu.hpp"
#include "opcodes.hpp"

// http://www.z80.info/decoding.htm

#define ROM_OFFSET 0x00 // Cartridge start at address 0

std::string input_path = "../rom/tetris.gb";

int main() {

  //Load ROM to memory

  uint8_t *p_rom_buffer;

  // Create input file stream
  std::ifstream input(input_path, std::ios::binary);

  if (!input.is_open()) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  input.seekg(0, std::ios::end);
  std::size_t size = input.tellg();

  std::cout << "ROM size: " << size << " bytes" << std::endl;

  std::filebuf *pbuf = input.rdbuf();

  pbuf->pubseekpos(0, input.in);

  // char *buffer = new char[size];
  CPU *cpu = new CPU();
  uint8_t *rom = cpu->m.rom;
  char * buffer = (char*)(rom+ROM_OFFSET);
  pbuf->sgetn(buffer, size);
  p_rom_buffer = (uint8_t *)buffer;

  input.close();
  
  int MAX_CYCLES = 12800;
  for (int cycles = 0; cycles < MAX_CYCLES;) {

    std::cout << cpu->PC << "\t";

    //Fetch
    uint8_t instruction = uint8_t(p_rom_buffer[cpu->PC]);
    // (cpu->PC)++

    // Decode + Run
    // Multiple ways to implement the dispatch loop :
    // - big switch statement
    // - computed go to
    // - ???
    int num_args = cpu->decode(instruction);
    
    
    // std::cout << cpu->PC;
    cycles += 1;
    cpu->PC += num_args;
    
  }

  std::cout << std::endl;
  std::cout << "TIMEOUT!" << std::endl;

  delete[] buffer;

  return 0;
}
