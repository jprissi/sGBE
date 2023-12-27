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

#define MAX_INSTRUCTIONS 1e8
bool debug_implementation = false;

std::string cartridge_path = "../rom/tetris.gb";
std::string serial_output;

bool early_stop = false;

bool log_to_tty = false;
bool log_to_file = false;
bool log_to_serial = false;

void read_serial(CPU *p_cpu)
{
    if (p_cpu->m.read(0xFF02) == 0x81)
    {
        serial_output = serial_output + std::string(1, p_cpu->m.read(0xFF01));
        p_cpu->m.write(0xFF02, 0x01);
        p_cpu->request_interrupt(INT_SERIAL);
    }
}

int main(int argc, char **argv)
{
    log_to_tty = true;
    log_to_file = true; // For use with gameboy-doctor
    log_to_serial = true;

    if (!log_to_tty)
        std::cout.setstate(std::ios_base::failbit); // Don't log to console

    if (argc > 1)
    {
        debug_implementation = true;
        cartridge_path = argv[1];
    }

    std::cout << "Using cartridge : " << cartridge_path << std::endl;

    CPU *p_cpu = new CPU(cartridge_path, debug_implementation);
    View *p_view = new View(p_cpu, debug_implementation);
    PPU *p_ppu = new PPU(p_cpu, &(p_cpu->m), p_view, debug_implementation);

    if (log_to_file)
        p_cpu->logfile.open("./cpu_log.txt");

    uint8_t timeout = 30;
    int i = 0;
    bool quit = false;
    while (!quit)
    {
        std::cout << std::endl << std::dec << std::setw(7) << (int)i << " ";
        
        quit = p_view->handle_SDL_events(*p_cpu);

        if (early_stop && i >= MAX_INSTRUCTIONS)
            quit = true;

        if (p_cpu->step_by_step and i>= 3063820)
        {
            p_cpu->print_registers();
            std::cin.ignore();
            // p_cpu->step_by_step = false;
        }

        if (!p_cpu->halt && log_to_file)
                p_cpu->log();

        // uint8_t opcode = p_cpu->m.read(p_cpu->PC);
        uint8_t opcode = p_cpu->fetch();
        uint8_t cycles = p_cpu->execute(opcode, log_to_file);

        /* Deal with serial transfer (Blargg's test rom)*/
        if (log_to_serial)
            read_serial(p_cpu);

        if (p_cpu->timeout == 0)
        {
            std::cout << "Reached loop" << std::endl;
            quit = true;
        }

        if (!debug_implementation)
            p_ppu->update(cycles);
        ++i;
    }

    if (log_to_file)
        p_cpu->logfile.close();

    std::cout << std::endl
              << "Timeout";
    std::cout.clear();
    std::cout << serial_output << std::endl;

    return 0;
}