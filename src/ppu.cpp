#include "ppu.hpp"
// #include "cpu.hpp"
#include <stdio.h>
#include <iostream>
#include <bitset>
#include "view.hpp"

/**
 * PPU - Pixel Processing Unit
 */

// Tile data ; all

// Tile Map : Background & Window
// OAM : objects

PPU::PPU(CPU *p_cpu, MemoryController *p_m, View *p_view, bool debug_implementation)
{
    set_memory_controller(p_m);

    this->renderer = p_view->app.renderer;
    this->cpu = p_cpu;

    p_LCDC = p_memory->get_pointer(LCDC);
    p_LY = p_memory->get_pointer(LY);

    p_SCX = p_memory->get_pointer(SCX);
    p_SCY = p_memory->get_pointer(SCY);

    p_color_palette_obj = p_memory->get_pointer(COLOR_PALETTE_OBJ);
    p_color_palette_bg = p_memory->get_pointer(COLOR_PALETTE_BG);

    uint8_t stat = p_memory->read(STAT);
    stat = (stat & ~3) | mode;
    p_memory->write(STAT, stat);

    // pf = new PixelFetcher();

    if (debug_implementation)
    {
        *p_LY = 0x90; // For Blargg
        // *p_LY = 0x94; // For tetris
    }
}

void PPU::OAM_scan()
{
    // Search for objects in current scan line (80 dots) and add them to the sprite_adresses array
    uint8_t scanline = *p_LY;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;

    num_sprites = 0;

    for (uint16_t sprite_address = OAM_START; sprite_address < 0xFE9F; sprite_address += 4)
    {
        if (num_sprites >= 10) // Maximum 10 sprites per line
            break;

        uint8_t y_pos = this->p_memory->read(sprite_address);
        uint8_t highest_y = y_pos;
        uint8_t lowest_y = y_pos + sprite_size_y;

        if (highest_y > *p_LY + (uint8_t)16 || lowest_y < *p_LY + (uint8_t)16)
            continue;

        sprites_adresses[num_sprites++] = sprite_address;
    }
}

void PPU::send_pixels()
{
    // Send pixels to LCD, video memory is locked (172 dots -> 289 dots)

    draw_bg_win = (*p_LCDC >> 0) & 1;
    draw_fg = (*p_LCDC >> 1) & 1;
    obj_mode = (*p_LCDC >> 2) & 1;

    // Background
    // draw_bg_win = false;
    if (draw_bg_win)
        this->draw_background_tiles();

    // draw_fg = false;
    // Sprites / Foreground
    if (draw_fg)
        this->draw_foreground_sprites();
}

void PPU::draw_background_tiles()
{
    uint8_t current_scanline = *p_LY;
    uint8_t tile_y = current_scanline / 8;
    uint8_t tile_offset_y = current_scanline - tile_y * 8;

    // Scroll values
    uint8_t scx, scy;
    scx = *p_SCX;
    scy = *p_SCY;

    for (int tile_x = 0; tile_x < 32; tile_x++)
    {
        uint8_t tile_id = p_memory->read(TILEMAP1_START + 32 * tile_y + tile_x);

        uint16_t tile_address; // Depends on adressing mode LCDC.4 (for BG & window)
        if ((*p_LCDC >> 4) & 1)
        {
            tile_address = TILESET_START + 16 * tile_id + 2 * tile_offset_y;
        }
        else
        {
            tile_address = TILESET_START_9000 + 16 * (int8_t)tile_id + 2 * tile_offset_y;
        }

        uint8_t tile_data_lsb = p_memory->read(tile_address);
        uint8_t tile_data_msb = p_memory->read(tile_address + 1);

        for (int tile_offset_x = 0; tile_offset_x < 8; ++tile_offset_x)
        {
            uint8_t x = tile_x * 8 + tile_offset_x - scx;
            uint8_t y = current_scanline - scy;

            uint8_t pixel_value_id = ((tile_data_lsb >> (7 - tile_offset_x)) & 1) | ((tile_data_msb >> (7 - tile_offset_x)) & 1) << 1;
            uint8_t color = (*p_color_palette_bg >> 2 * pixel_value_id) & 3;

            if (color != 0)
            {
                set_color(color);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

void PPU::set_color(uint8_t color_id)
{
    SDL_Color color = palette[color_id];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
}

void PPU::draw_all_sprites()
{
    uint8_t x_pos = 0;
    uint8_t y_pos = 0;
    uint8_t xsize = 8;
    uint8_t ysize = obj_mode ? 16 : 8;

    uint8_t tile_index = 0;
    uint8_t current_scanline = *p_LY;

    uint8_t tile_row = *p_LY / ysize;
    for (uint16_t sprite_address = OAM_START; sprite_address < 0xFE9F; sprite_address += 4)
    {
        // 40 times
        // 160x144 -> 20 x 18 8x8 objects
        uint8_t tile_offset_y = current_scanline - y_pos * ysize;
        uint16_t tile_address = TILESET_START + tile_row * (20 - 1) * 2 * 8 + tile_index * 16 + 2 * tile_offset_y;
        draw_tile(tile_address, x_pos, *p_LY);

        tile_index++;
        x_pos += 8;
        // y_pos += 8;

        if (x_pos > 160)
        {
            y_pos += 8 * (x_pos / 160);
            x_pos = x_pos % 160;
            break;
        }
    }
}

void PPU::draw_foreground_sprites()
{

    // Object attributes in $FE00-FE9F
    // 1 sprite = 4 bytes => 40 sprites on screen
    // Byte 0 = Y pos + 16

    // Sprite size 8x8 or 8x16
    uint8_t sprite_size_x = 8;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;

    uint8_t current_scanline = *p_LY;

    for (uint8_t i = 0; i < num_sprites; ++i)
    {
        uint16_t sprite_address = sprites_adresses[i];

        uint8_t y_pos = p_memory->read(sprite_address, true);
        uint8_t x_pos = p_memory->read(sprite_address + 1, true);

        draw_sprite(sprite_address, x_pos, y_pos);
    }
}

void PPU::draw_tile(uint16_t tile_address, uint8_t x_pos, uint8_t y_pos)
{

    uint8_t tile_data_lsb = p_memory->read(tile_address, true);
    uint8_t tile_data_msb = p_memory->read(tile_address + 1, true);

    for (int tile_offset_x = 0; tile_offset_x < 8; ++tile_offset_x)
    {
        uint8_t x = x_pos + tile_offset_x - 8; // xpos = x+8

        uint8_t pixel_value_id = ((tile_data_lsb >> (7 - tile_offset_x)) & 1) | ((tile_data_msb >> (7 - tile_offset_x)) & 1) << 1;
        uint8_t color = (*p_color_palette_obj >> 2 * pixel_value_id) & 3;
        if (color != 0)
        {
            set_color(color);
            SDL_RenderDrawPoint(renderer, x, y_pos - 16);
        }
    }
}

void PPU::draw_sprite(uint16_t sprite_address, uint8_t x_pos, uint8_t y_pos)
{
    uint8_t tile_index = this->p_memory->read(sprite_address + 2);
    uint8_t current_scanline = *p_LY;
    uint16_t tile_offset_y = (uint16_t)current_scanline - (y_pos - 16);
    uint16_t tile_address = TILESET_START + tile_index * 16 + 2 * tile_offset_y;

    this->draw_tile(tile_address, x_pos, y_pos + tile_offset_y);
}

void PPU::wait_h()
{
    // 376 dots - mode3 length
    // *p_LY = (*p_LY + 1) % 153;
    if (*p_LY == 143)
    {
        SDL_RenderPresent(renderer);
        cpu->request_interrupt(INT_VBLANK);
        mode = VERTICAL_BLANK;
    }
    else
    {
        (*p_LY)++;
        mode = OAM_SCAN;
    };
}

void PPU::wait_v()
{
    // 4560 dots (10 scanlines)
    if (*p_LY == 154) // 153?
    {
        // std::cout << "Current FPS: " << std::to_string(1.0f / (2 * 16.666f / 1000)) << std::endl;
        // std::cout << "Cycles: " << std::to_string(cycles) << std::endl; // 4.19MHz
        float elapsed = (SDL_GetPerformanceCounter() - last_frame_timestamp) / (float)SDL_GetPerformanceFrequency();
        float elapsedMS = elapsed * 1000.0f;

        std::cout.clear();
        std::cout << "Current FPS: " << std::to_string(1.0f / (elapsed)) << std::endl;
        std::cout.setstate(std::ios_base::failbit);

        if (16.666f - elapsedMS > 0)
        {
            // Soft limit to 60Hz for game logic (if not enforced by SDL using preventvsync flag)
            SDL_Delay(floor(16.666f - elapsedMS));
        }


        last_frame_timestamp = SDL_GetPerformanceCounter();
        SDL_SetRenderDrawColor(renderer, 0xE0, 0xF8, 0xD0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        *p_LY = 0;
        mode = OAM_SCAN;
        return;
    }
    (*p_LY)++;
}

void PPU::switch_mode()
{
    cycles_elapsed -= durations[mode];
    switch (mode)
    {
    case HORIZONTAL_BLANK:
        wait_h();
        break;
    case VERTICAL_BLANK:
        wait_v();
        break;
    case OAM_SCAN:
        OAM_scan();
        mode = DRAWING_PIXELS;
        break;
    case DRAWING_PIXELS:
        send_pixels();
        mode = HORIZONTAL_BLANK;
        break;
    };

    // Update STAT lowest bits (TODO: generate interrupt if necessary)
    uint8_t stat = p_memory->read(STAT);
    stat = (stat & ~3) | mode;
    p_memory->write(STAT, stat);
}

void PPU::update(uint8_t cpu_cycles)
{
    uint8_t status = *p_LCDC; // LCD + PPU ON/OFF
    bool enabled = (status & (1 << 7));
    if (!enabled) // Disabled for now
        return;

    cycles_elapsed += cpu_cycles;
    if (cycles_elapsed >= durations[mode]) // Wait for enough cycles before switching
        switch_mode();
}

void PPU::set_memory_controller(MemoryController *p_m)
{
    p_memory = p_m;
}

// /** Pixel Fetcher **/
// void PixelFetcher::get_tile()
// {
//     // x = ((SCX / 8) + x) & $1F;
//     // y = (y + SCY) & 255;
// }

// void PixelFetcher::get_data()
// {
// }

// void PixelFetcher::clear_queues()
// {
//     obj_FIFO.clear();
//     bgd_FIFO.clear();
// }
