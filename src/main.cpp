#include <stdio.h>
#include <cstddef>
#include <stdint.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <bitset>

#include "cpu.hpp"
#include "ppu.hpp"
#include "opcodes.hpp"
#include "view.hpp"

#define ROM_OFFSET 0x00 // Cartridge start at address 0

#define MAX_INSTRUCTIONS 1e7

// bool debug_implementation = true;
bool debug_implementation = false;

// std::string cartridge_path = "../tools/gb-test-roms/cpu_instrs/cpu_instrs.gb";
// std::string cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/01-special.gb";
std::string cartridge_path = "../rom/tetris.gb";
std::string serial_output;

bool log_to_tty = false;
bool log_to_file = false;

void read_serial(CPU *p_cpu)
{
    // MemoryController mem = p_cpu->m;
    if (p_cpu->m.read(0xFF02) == 0x81)
    {
        serial_output = serial_output + std::string(1, p_cpu->m.read(0xFF01));
        p_cpu->m.write(0xFF02, 0x01);
        p_cpu->request_interrupt(INT_SERIAL);
    }
}

int main(int argc, char **argv)
{

    // log_to_tty = true;
    // log_to_file = true; // For use with gameboy-doctor

    if (!log_to_tty)
        std::cout.setstate(std::ios_base::failbit); // Don't log to console

    if (argc > 1)
    {
        debug_implementation = true;
        cartridge_path = argv[1];
    }

    std::cout << "Using cartridge : " << cartridge_path << std::endl;

    CPU *p_cpu = new CPU(cartridge_path, debug_implementation);
    p_cpu->log_file.open("./cpu_log.txt");

    View *p_view = new View(p_cpu, debug_implementation);
    PPU *ppu = new PPU(p_cpu, &(p_cpu->m), p_view, debug_implementation);

    uint8_t timeout = 30;
    int i = 0;
    bool quit = false;

    while (!quit) //&& i <= MAX_INSTRUCTIONS)
    {
        quit = p_view->handle_SDL_events(*p_cpu);

        if (p_cpu->step_by_step)
        {
            std::cout.clear();
            p_cpu->print_registers();
            std::cin.ignore();
            // cpu->step_by_step = false;
        }

        uint8_t opcode = p_cpu->m.read(p_cpu->PC);
        uint8_t cycles = p_cpu->step(opcode, log_to_file);

        /* Deal with serial transfer (Blargg's test rom)*/
        read_serial(p_cpu);

        // if (p_cpu->timeout == 0)
        // {
        //     std::cout << "Reached loop" << std::endl;
        //     quit = true;
        // }

        if (!debug_implementation)
        {
            ppu->update(cycles);
        }

        ++i;
    }

    p_cpu->log_file.close();

    std::cout << std::endl;
    std::cout << "Timeout";
    std::cout.clear(); // Log serial output to console
    std::cout << serial_output << std::endl;

    return 0;
}