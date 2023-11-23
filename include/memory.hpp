#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <string>
#include <stdint.h>

#define ROM_SIZE 1024 * 1024 * 8 // 8MiB

class MemoryController
{

private:
    // uint8_t get_instruction(uint8_t pc);

    uint8_t rom[ROM_SIZE];
    uint8_t boot_rom[8 * 256]; // 256B
    void copy_file_to_memory(std::string &file_path, bool is_boot = false);

public:
    bool boot_rom_enabled = true;

    MemoryController();

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);
    uint16_t read16(uint16_t address);

    uint8_t *get_pointer(uint16_t address);
    void load_cartridge(std::string &file_path);

    void DMA_transfer();
};

#endif