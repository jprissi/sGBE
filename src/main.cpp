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

#define ROM_OFFSET 0x00     // Cartridge start at setress 0
#define CLOCK_SPEED 4194304 // 4.194304MHz

#define MAX_INSTRUCTIONS 1e6

bool log_to_file = false;
// bool debug_implementation = true;
bool debug_implementation = false;

std::string cartridge_path = "../rom/tetris.gb";

int main()
{
    if (debug_implementation)
    {
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/01-special.gb";
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"; // Need timer interrupt
        cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/05-op rp.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";

        log_to_file = true;
    }

    CPU *cpu = new CPU(cartridge_path, debug_implementation);

    if (debug_implementation)
    {
        cpu->m.write((uint16_t)0xFF44, (uint8_t)0x90);
    }

    // cpu->set_breakpoint(0x2817); // Starts loading tiles
    // cpu->set_breakpoint(0x0100); // End of boot rom
    // cpu->set_breakpoint(0x0027); // Start decompressing nintendo logo
    // cpu->set_breakpoint(0x0034); // Finished decompressing
    // cpu->set_breakpoint(0x089); // Setup display
    // cpu->set_breakpoint(0x0e9); // Check Nintendo logo
    cpu->set_breakpoint(0x204); // Bug?

    View *p_view = new View(cpu);
    PPU *ppu = new PPU(cpu, &cpu->m);

    int i = 0;
    int quit = 0;

    std::ofstream log_file;
    log_file.open("./cpu_log.txt");

    Uint64 start = SDL_GetPerformanceCounter();
    uint16_t cycles = 0;
    int timeout = 10;

    while (!quit && i <= MAX_INSTRUCTIONS)
    {
        quit = p_view->handle_SDL_events();

        if (cpu->step_by_step)
        {
            cpu->print_registers();
            // quit=true;
            std::cin.ignore();
            cpu->step_by_step = false;
        }

        if (log_to_file)
        {
            cpu->log(log_file);
        }

        std::cout << "[" << std::hex << std::setw(4) << cpu->PC << "]\t"; // Print PC (1st number on line)

        int instruction = int(cpu->m.read(cpu->PC));
        cycles += cpu->step((uint8_t)instruction);

        if (instruction == int(cpu->m.read(cpu->PC)))
        {
            timeout--;
            if (timeout == 0)
            {
                std::cout << "reached loop, ending" << std::endl;
                quit = true;
            }
        }
        else
        {
            timeout = 10;
        }

        if (!debug_implementation)
        {
            ppu->update(cycles);
            cycles = 0;
        }
        ++i;
    }

    std::cout << std::endl << "Timeout";
    log_file.close();

    return 0;
}