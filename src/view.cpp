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

SDL_Window *View::initWindow()
{
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

    return window;
}

SDL_Renderer *View::initGraphics(SDL_Window *window)
{
    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    render_flags |= SDL_RENDERER_PRESENTVSYNC; // 60Hz

    // creates a renderer to render our images
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, render_flags);
    SDL_RenderSetScale(renderer, SCALE, SCALE); // 2x upscale

    SDL_SetRenderDrawColor(renderer, 0xE0, 0xF8, 0xD0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    return renderer;
}

bool View::handle_SDL_events(CPU &cpu)
{

    SDL_Event event;
    bool quit = false;

    // State is saved using two 8 bit variables, set to 1 if false and 0 if true
    uint8_t input_a = 0x0F;
    uint8_t input_b = 0x0F;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_AMPERSAND:
                p_cpu->step_by_step = !p_cpu->step_by_step;
                break;
            case SDLK_c:
                p_cpu->step_by_step = false;
                break;
            case SDLK_a:
                input_a &= ~(1 << 0);
                break;
            case SDLK_b:
                input_a &= ~(1 << 1);
                break;
            case SDLK_UP:
                input_b &= ~(1 << 2);
                break;
            case SDLK_s: // Select
                input_a &= ~(1 << 2);
                break;
            case SDLK_RIGHT:
                input_b &= ~(1 << 0);
                break;
            case SDLK_LEFT:
                input_b &= ~(1 << 1);
                break;
            case SDLK_DOWN:
                input_b &= ~(1 << 3);
                break;
            case SDLK_SPACE: // start
                input_a &= ~(1 << 3);
                break;
            default:
                break;
            };
            cpu.m.set_joypad(input_a, input_b);
            cpu.request_interrupt(INT_JOYPAD);
            break;
        case SDL_KEYUP:
            cpu.m.set_joypad(0x0F, 0x0f);
            break;
        case SDL_QUIT:
            quit = true;
            break;
        };
    }
    return quit;
}