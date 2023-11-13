#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include "memory.hpp"
#include <queue>
#include <stdio.h>
#include <SDL2/SDL.h>

#define SCANLINES 154
#define DOT 2 ^ (-22) // Seconds
#define FPS 59.7      // Hertz

// https://gbdev.io/pandocs/Rendering.html
// https://rylev.github.io/DMG-01/public/book/graphics/tile_ram.html

#define LCDC 0xFF40 // LCDC

#define SCY 0xFF42  // ScrollY
#define SCX 0xFF43  // ScrollX

#define LY 0xFF44   // Current scanline (R)

#define TILEMAP1_START 0x9800 // -$9BFF
#define TILEMAP2_START 0x9C00 // -$9BFF

#define TILESET_START 0x8000

#define COLOR_PALETTE 0xFF47

enum ppu_mode
{
    HORIZONTAL_BLANK,
    VERTICAL_BLANK,
    OAM_SCAN,
    DRAWING_PIXELS
};

enum tile
{
    WINDOW,
    BACKGROUND,
    SPRITE
};

class PixelFetcher
{
    // https://gbdev.io/pandocs/pixel_fifo.html

    std::deque<uint8_t> obj_FIFO;
    std::deque<uint8_t> bgd_FIFO;
    uint8_t x, y;
    uint16_t tilemap_address = 0x9800; // Sometimes different

public:
    void get_tile();
    void get_data();
    void push();

    void clear_queues(); // Called at the beginning of mode 3
};

class PPU
{
    uint8_t durations[4] = {51, 144, 20, 43};

private:
    ppu_mode mode = HORIZONTAL_BLANK;
    // int ly = 0x00; // 0-153 and 00h when off

    uint8_t *p_LCDC; // LCDC
    uint8_t *p_SCX;  // Scroll Y
    uint8_t *p_SCY;  // Scroll Y
    uint8_t *p_LY;

    PixelFetcher *pf;

    uint8_t *p_colPalette;

    uint8_t cycles_elapsed = 0;
    MemoryController *p_memory;

public:
    SDL_Renderer *renderer;
    PPU(MemoryController *p_m, SDL_Renderer* renderer);

    void set_memory_controller(MemoryController *p_m);
    void OAM_scan();
    void send_pixels();
    void wait_h();
    void wait_v();
    void switch_to_next_mode();

    void update(uint8_t cpu_cycles);
};

#endif