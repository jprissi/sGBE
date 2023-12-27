#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <string>
#include <stdint.h>

#define ROM_SIZE 1024 * 1024 * 8 // 8MiB
#define LCDC 0xFF40              // LCDC // Moved to memory

struct
{
    uint8_t input_a = 0x0F;
    uint8_t input_b = 0x0F;
} joypad_registers;

class MemoryController
{

private:
    // Joypad joypad;
    uint8_t rom[ROM_SIZE];
    uint8_t boot_rom[8 * 256]; // 256 Bytes
    std::string boot_rom_path = "../rom/dmg_boot.bin";
    bool boot_rom_enabled = true;

    void copy_file_to_memory(std::string &file_path, bool is_boot = false);
    uint8_t read_IO_register(uint16_t address);

    uint8_t read_vram(uint16_t address, bool is_ppu);
    uint8_t read_oam(uint16_t address);
    void write_vram(uint16_t address, uint8_t value);
    void write_oam(uint16_t address, uint8_t value);

    void DMA_transfer();

public:
    MemoryController();

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address, bool is_ppu = false);
    uint8_t *get_pointer(uint16_t address);
    void load_cartridge(std::string &file_path);

    uint8_t read_joypad();
    void set_joypad(uint8_t a, uint8_t b);

    void enable_bootrom();
    void disable_bootrom();
};

#endif