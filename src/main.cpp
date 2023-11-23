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

#define MAX_INSTRUCTIONS 1e7

// bool debug_implementation = true;
bool debug_implementation = false;

int main(int argc, char** argv)
{
    std::string cartridge_path = "../rom/tetris.gb";
    // bool log_to_console = true;
    if (argc > 1){
        debug_implementation = true;
        // log_to_console = false;
        // std::cout.setstate(std::ios_base::failbit); // Don't log to console
        cartridge_path = argv[1];
    }
    std::cout << "Using cartridge : " << cartridge_path << std::endl;

    bool log_to_file = false;
    if (debug_implementation)
    {
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/01-special.gb";
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"; // Need timer interrupt
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/05-op rp.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"; // OK!
        // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";

        log_to_file = true;
    }

    CPU *cpu = new CPU(cartridge_path, debug_implementation);

    // cpu->set_breakpoint(0x2817); // Starts loading tiles
    // cpu->set_breakpoint(0x0100); // End of boot rom
    // cpu->set_breakpoint(0x0027); // Start decompressing nintendo logo
    // cpu->set_breakpoint(0x0034); // Finished decompressing
    // cpu->set_breakpoint(0x089); // Setup display
    // cpu->set_breakpoint(0x0e9); // Check Nintendo logo
    // cpu->set_breakpoint(0xc246); // Bug?
    // cpu->set_breakpoint(0x0001); // Bug? 

    View *p_view = new View(cpu);
    PPU *ppu = new PPU(cpu, &cpu->m, p_view, debug_implementation);
    
    std::ofstream log_file;
    log_file.open("./cpu_log.txt");

    // Uint64 start = SDL_GetPerformanceCounter();
    uint8_t cycles = 0;
    uint8_t timeout = 30;

    int i = 0;
    bool quit = false;
    std::string serial_output;
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

        if (log_to_file) {cpu->log(log_file);}

        std::cout << std::hex << "[0x"  << std::setw(4) << cpu->PC << "]\t";

        uint8_t opcode = cpu->m.read(cpu->PC);
        cycles += cpu->step(opcode);

        // int i=0;
        // while (true){
        //     uint8_t val = cpu->m.read(0xA000 + i);
        //     // std::cout << std::hex << (int)val << "; ";
        //     if (val == 0x00){
        //         break;
        //     }
        //     i++;
        // }

        uint8_t serial_val = cpu->m.read(0xFF02);
        if(serial_val == 0x81){
            uint8_t val = cpu->m.read(0xFF01);
            // std::cout << "serial: " << std::hex << (int)val << std::endl;
            // std::cout << std::dec << val << std::endl;
            serial_output = serial_output + std::string(1, val);
            cpu->m.write(0xFF02, 0x01);
            cpu->request_interrupt(INT_SERIAL);
            // std::cin.ignore();
        }
        // 0x81 written to address 0xFF02, then log the content of address 0xFF01.

        // timer.update(cycles);

        if (opcode == int(cpu->m.read(cpu->PC)))
        {
            timeout--;
            if (timeout == 0)
            {
                std::cout << "Reached loop" << std::endl;
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
    std::cout << serial_output << std::endl;
    return 0;
}