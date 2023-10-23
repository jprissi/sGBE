#include <stdio.h>
#include <string>
#include <stdint.h>

#define ROM_SIZE 1024 * 1024 * 8 // 8MiB

class Memory
{

    
    // void load_cartridge(std::string path);
    uint8_t get_instruction(uint8_t pc);

public:
    Memory();
    uint8_t rom[ROM_SIZE];
};