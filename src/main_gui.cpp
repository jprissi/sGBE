#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// #include <SDL2/SDL_timer.h>
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
// #include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>

#include "cpu.hpp"
#include "ppu.hpp"
#include "opcodes.hpp"

#define ROM_OFFSET 0x00 // Cartridge start at setress 0

#define SCALE 3
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define CLOCK_SPEED 4194304 // 4.194304MHz

typedef struct
{
    SDL_Renderer *renderer;
    SDL_Window *window;
} App;

void render_tilemap(CPU &cpu, SDL_Renderer *renderer)
{
    // https://gbdev.io/pandocs/Tile_Data.html

    SDL_SetRenderDrawColor(renderer, 226, 243, 228, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    uint16_t i = 0;
    uint16_t start = 0x8000; // 0x80d0;
    uint16_t end = start + 48 * 16;
    for (uint16_t setress = start; setress <= end; setress += 16) // Iterate over each block
    {
        i = setress - start;
        std::cout << setress << std::endl;
        uint16_t block_idx = i / 16;
        std::cout << block_idx << std::endl;
        uint16_t block_x = block_idx % 16;
        uint16_t block_y = block_idx / 16;

        // Block r, l
        for (int row = 0; row < 8; row++)
        {
            // Each line has 2 bytes
            uint16_t value_lsb = cpu.m.read(setress + 2 * row);
            uint16_t value_msb = cpu.m.read(setress + 2 * row + 1);

            for (int col = 0; col < 8; col++)
            {
                int value = (value_lsb & (1 << (7 - col))) >> (7 - col) | (value_msb & (1 << (7 - col))) >> (7 - col) << 1;
                if (value != 0)
                {
                    int x = block_x * 8 + col;
                    int y = block_y * 8 + row;
                    SDL_RenderDrawPoint(renderer, x, y);
                }
                std::cout << value << " ";
            }

            std::cout << std::endl;
        }
    }
    SDL_RenderPresent(renderer);
}

SDL_Window *initWindow()
{
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("SGBE", // creates a window
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH * SCALE,
                                          SCREEN_HEIGHT * SCALE, 0);
    // SDL_SetWindowBordered(window, SDL_TRUE);

    return window;
}
SDL_Renderer *initGraphics(SDL_Window *window)
{
    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, render_flags);
    SDL_RenderSetScale(renderer, SCALE, SCALE); // 2x upscale

    // #332c50
    // #46878f
    // #94e344

    // #e2f3e4
    SDL_SetRenderDrawColor(renderer, 226, 243, 228, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // for (int i = 0; i < SCREEN_WIDTH; ++i)
    // {
    //     SDL_RenderDrawPoint(renderer, i, i);
    // }
    // SDL_RenderPresent(renderer);

    return renderer;
}

int main()
{
    SDL_Window *window = initWindow();
    SDL_Renderer *renderer = initGraphics(window);

    std::string cartridge_path;
    bool debug_implementation = true;
    // bool debug_implementation = false;
    bool log_to_file;

    cartridge_path = "../rom/tetris.gb";

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

    // cpu->set_breakpoint(0x2817); // Starts loading tiles
    // cpu->set_breakpoint(0x0100); // End of boot rom
    // cpu->set_breakpoint(0x0027); // Start decompressing nintendo logo
    // cpu->set_breakpoint(0x0034); // Finished decompressing
    // cpu->set_breakpoint(0x089); // Setup display
    // cpu->set_breakpoint(0x0e9); // Check Nintendo logo

    cpu->set_breakpoint(0x204); // Bug?

    if (debug_implementation)
    {
        cpu->m.write((uint16_t)0xFF44, (uint8_t)0x90);
    }

    PPU *ppu = new PPU(cpu, &cpu->m, renderer);
    ppu->set_memory_controller(&(cpu->m));

    int MAX_INSTRUCTIONS = 10000000;
    int i = 0;
    int quit = 0;
    SDL_Event event;

    std::ofstream log_file;
    log_file.open("./cpu_log.txt");

    Uint64 start = SDL_GetPerformanceCounter();
    uint16_t cycles = 0;
    int timeout = 10;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_SPACE:
                    cpu->step_by_step = !cpu->step_by_step;
                    break;
                case SDLK_c:
                    cpu->step_by_step = false;
                default:
                    break;
                };
                break;
            case SDL_QUIT:
                quit = true;
                break;
            };
            // else if (event.type == SDL_KEYDOWN and cpu->step_by_step)
            // {
            // }
        }

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

        // while (cycles <= 16.666f / 1000 * 2 * CLOCK_SPEED) { // 4Mhz * 16ms
        std::cout << "[" << std::hex << std::setw(4) << cpu->PC << "]\t"; // Print PC (1st number on line)

        // std::cout << "reading instruction" << std::endl;
        int instruction = int(cpu->m.read(cpu->PC));
        // std::cout << "executing instruction : " << std::hex << (int)instruction << std::endl;
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

        // }

        if (!debug_implementation)
        {
            // Uint64 end = SDL_GetPerformanceCounter();
            // float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();

            // float elapsedMS = elapsed * 1000.0f;

            // if (elapsedMS > 16.666f)
            // {
            // Cap to 60 FPS
            // if (2 * 16.666f - elapsedMS > 0)
            // {
            //     SDL_Delay(floor(2 * 16.666f - elapsedMS));
            // }

            // start = SDL_GetPerformanceCounter();

            ppu->update(cycles);

            // std::cout << "Current FPS: " << std::to_string(1.0f / (2 * 16.666f / 1000)) << std::endl;
            // std::cout << "Cycles: " << std::to_string(cycles) << std::endl; // 4.19MHz

            // cycles -= 16 * 2 * 4;
            cycles = 0;
            // }
        }

        if (i >= MAX_INSTRUCTIONS)

        {
            quit = 1;
        }

        // SDL_PumpEvents();

        ++i;
    }

    std::cout << std::endl
              << "Timeout";

    log_file.close();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // cpu->crash();

    return 0;
}