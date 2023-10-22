#include <stdio.h>
// #include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "opcodes.hpp"

// http://www.z80.info/decoding.htm

int main() {
  std::string input_path = "../rom/tetris.gb";

  // Create input file stream
  std::ifstream input(input_path, std::ios::binary);

  if (!input.is_open()) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  input.seekg(0, std::ios::end);
  std::size_t size = input.tellg(); // pbuf->pubseekoff(0, input.end, input.in);

  std::cout << "ROM size: " << size << "bytes" << std::endl;

  char *buffer = new char[size];
  std::filebuf *pbuf = input.rdbuf();

  pbuf->pubseekpos(0, input.in);
  pbuf->sgetn(buffer, size);
  input.close();

  //   std::cout.write(buffer, size);
  //   std::cout << std::endl;

  //   std::cout << (unsigned int) buffer[0] << std::endl;

  unsigned char *pbuffer = (unsigned char *)buffer;

  for (int pc = 0; pc < size; ++pc) {
    int instruction = int(pbuffer[pc]);
    decode(instruction);
    std::cout << std::setw(2) << std::hex << std::setfill('0')
              << int(pbuffer[pc]) << " ";
  }

  std::cout << std::endl;

  //   std::cout << int(pbuffer[0]) << std::endl;

  delete[] buffer;

  return 0;
}
