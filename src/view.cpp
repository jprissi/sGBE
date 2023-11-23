#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "cpu.hpp"
#include "view.hpp"

View::View(CPU *p_cpu, bool debug_implementation)
{
    this->debug = debug_implementation;
    this->p_cpu = p_cpu;
    if (debug_implementation)
        return;
    SDL_Window *window = initWindow();
    this->app.renderer = initGraphics(window);
    this->app.window = window;
};

View::~View()
{
    if (this->debug)
        return;
    SDL_DestroyRenderer(this->app.renderer);
    SDL_DestroyWindow(this->app.window);
    SDL_Quit();
}

void View::render_tilemap(CPU &cpu, SDL_Renderer *renderer)
{
    // https://gbdev.io/pandocs/Tile_Data.html

    SDL_SetRenderDrawColor(renderer, 0xE0, 0xF8, 0xD0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, SDL_ALPHA_OPAQUE);

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

SDL_Window *View::initWindow()
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
SDL_Renderer *View::initGraphics(SDL_Window *window)
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

    SDL_SetRenderDrawColor(renderer, 0xE0, 0xF8, 0xD0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, SDL_ALPHA_OPAQUE);

    return renderer;
}

bool View::handle_SDL_events()
{

    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
                p_cpu->step_by_step = !p_cpu->step_by_step;
                break;
            case SDLK_c:
                p_cpu->step_by_step = false;
            default:
                break;
            };
            break;
        case SDL_QUIT:
            quit = true;
            break;
        };
    }
    return quit;
}