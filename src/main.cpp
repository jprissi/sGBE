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

std::string input_path = "../rom/tetris.gb";

int main() {

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

  char *buffer = new char[size];
  pbuf->sgetn(buffer, size);
  p_rom_buffer = (uint8_t *)buffer;

  input.close();
  cpu cpu;
  for (int pc = 0; pc < size; ++pc) {
    int instruction = int(p_rom_buffer[pc]);
	std::cout << pc << "\t";
    pc += cpu.decode((uint8_t)instruction);
    // break;
    // std::cout << std::setw(2) << std::hex << std::setfill('0')
    //           << int(p_rom_buffer[pc]) << " ";
  }

  std::cout << std::endl;

  delete[] buffer;

  return 0;
}
