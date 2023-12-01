#ifndef PPU_H
#define PPU_H

#include <cstdint>
// #include <queue>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "cpu.hpp"
#include "view.hpp"
#include "memory.hpp"

#define SCANLINES 154
#define DOT 2 ^ (-22) // Seconds
#define FPS 59.7      // Hertz

// https://gbdev.io/pandocs/Rendering.html
// https://rylev.github.io/DMG-01/public/book/graphics/tile_ram.html

#define LCDC 0xFF40 // LCDC 
#define STAT 0xFF41

#define SCY 0xFF42 // ScrollY
#define SCX 0xFF43 // ScrollX

#define LY 0xFF44 // Current scanline (R)

#define TILEMAP1_START 0x9800 // -$9BFF
#define TILEMAP2_START 0x9C00 // -$9BFF

#define TILESET_START 0x8000
#define TILESET_START_9000 0x9000

#define OAM_START 0xFE00

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

class PPU
{
    uint8_t durations[4] = {51, 144, 20, 43};

private:
    void draw_all_sprites();

    uint16_t sprites_adresses[10];
    uint8_t num_sprites;
    // const SDL_Point p_points[SCREEN_WIDTH*SCREEN_HEIGHT];

    Uint64 last_frame_timestamp;

    ppu_mode mode = HORIZONTAL_BLANK;
    // int ly = 0x00; // 0-153 and 00h when off

    uint8_t *p_LCDC; // LCDC
    uint8_t *p_SCX;  // Scroll Y
    uint8_t *p_SCY;  // Scroll Y
    uint8_t *p_LY;

    bool obj_mode, draw_fg, draw_bg_win;

    void draw_sprite(uint16_t address, uint8_t x_pos, uint8_t y_pos);
    void draw_tile(uint16_t tile_address, uint8_t x_pos, uint8_t y_pos);
    void draw_foreground_sprites();
    void draw_background_tiles();

    uint8_t *p_color_palette;

    uint16_t cycles_elapsed = 0;

    MemoryController *p_memory;
    CPU *cpu;
    SDL_Renderer *renderer;

public:
    PPU(CPU *cpu, MemoryController *p_m, View *p_view, bool debug_implementation);

    void set_memory_controller(MemoryController *p_m);
    void OAM_scan();
    void send_pixels();
    void wait_h();
    void wait_v();
    void switch_to_next_mode();

    void update(uint8_t cpu_cycles);
};

// class PixelFetcher
// {
//     // https://gbdev.io/pandocs/pixel_fifo.html

//     std::deque<uint8_t> obj_FIFO;
//     std::deque<uint8_t> bgd_FIFO;
//     uint8_t x, y;
//     uint16_t tilemap_address = 0x9800; // Sometimes different

// public:
//     void get_tile();
//     void get_data();
//     // void push();

//     void clear_queues(); // Called at the beginning of mode 3
// };


#endif