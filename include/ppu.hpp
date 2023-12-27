#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "cpu.hpp"
#include "view.hpp"
#include "memory.hpp"

#define SCANLINES 154
#define DOT 2 ^ (-22) // Seconds
#define FPS 59.7      // Hertz

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

#define COLOR_PALETTE_BG 0xFF47
#define COLOR_PALETTE_OBJ 0xFF48

// https://gbdev.io/pandocs/Rendering.html
// https://rylev.github.io/DMG-01/public/book/graphics/tile_ram.html

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
private:
    ppu_mode mode = HORIZONTAL_BLANK;
    uint8_t modes_durations[4] = {51, 144, 20, 43};

    uint16_t sprites_adresses[10];
    uint8_t num_sprites;
    uint64_t timestamp_previous_frame;
    uint16_t cycles_elapsed = 0;

    uint8_t *p_LCDC; // LCDC
    uint8_t *p_SCX;  // Scroll Y
    uint8_t *p_SCY;  // Scroll Y
    uint8_t *p_LY;

    uint8_t *p_color_palette_bg;
    uint8_t *p_color_palette_obj;

    bool obj_mode, draw_fg, draw_bg_win;

    void draw_all_sprites();
    void draw_sprite(uint16_t address, uint8_t x_pos, uint8_t y_pos);
    void draw_tile(uint16_t tile_address, uint8_t x_pos, uint8_t y_pos);
    void draw_foreground_sprites();
    void draw_background_tiles();
    void set_color(uint8_t color);

    MemoryController *p_memory;
    CPU *cpu;
    SDL_Renderer *renderer;

    SDL_Color white = {0xe0, 0xf8, 0xd0};
    SDL_Color light_gray = {0x88, 0xC0, 0x70};
    SDL_Color dark_gray = {0x34, 0x68, 0x56};
    SDL_Color black = {0x08, 0x18, 0x20};

    SDL_Color palette[4] = {white, light_gray, dark_gray, black};

public:
    PPU(CPU *cpu, MemoryController *p_m, View *p_view, bool debug_implementation);

    void update(uint8_t cpu_cycles);
    void switch_mode();

    void OAM_scan();
    void send_pixels();
    void wait_h();
    void wait_v();
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