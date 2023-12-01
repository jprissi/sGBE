#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "cpu.hpp"

#define SCALE 3
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

typedef struct
{
    SDL_Renderer *renderer;
    SDL_Window *window;
} App;

class View
{
public:
    bool debug;
    App app;
    View(CPU *p_cpu, bool debug_implementation);
    ~View();
    CPU *p_cpu;
    void render_tilemap(CPU &cpu, SDL_Renderer *renderer);
    SDL_Window *initWindow();
    SDL_Renderer *initGraphics(SDL_Window *window);
    bool handle_SDL_events(CPU &cpu);
};

#endif