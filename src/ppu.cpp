#include "ppu.hpp"
// #include "cpu.hpp"
#include <stdio.h>
#include <iostream>
#include <bitset>
#include "view.hpp"

/**
 * PPU - Pixel Processing Unit
 */

PPU::PPU(CPU *p_cpu, MemoryController *p_m, View *p_view, bool debug_implementation)
{
    set_memory_controller(p_m);

    this->renderer = p_view->app.renderer;
    this->cpu = p_cpu;

    p_LCDC = p_memory->get_pointer(LCDC);
    p_LY = p_memory->get_pointer(LY);
    // p_memory->write((uint8_t)LY, (uint8_t)0x90); // Debug mode

    p_SCX = p_memory->get_pointer(SCX);
    p_SCY = p_memory->get_pointer(SCY);

    p_colPalette = p_memory->get_pointer(COLOR_PALETTE);
    // pf = new PixelFetcher();

    // *p_LY = 0x94; // For tetris
    // *p_LY = 0x90; // For Blargg
}

void PPU::OAM_scan()
{
    // Search for objects in current scan line
    //  80 dots
}

void PPU::send_pixels()
{
    // Send pixels to LCD (video memory locked)
    // 172 dots -> 289 dots

    bool draw_bg_win, draw_fg = false;
    this->obj_mode = (*p_LCDC >> 2) & 1; // 0 or 1;

    if (*p_LCDC & (1 << 0))
    {
        draw_bg_win = true;
    }
    if (*p_LCDC & (1 << 1))
    {
        draw_fg = true;
    }

    // Background
    if (draw_bg_win)
    {
        this->draw_background();
    }

    // Sprites / Foreground
    if (draw_fg)
        this->draw_foreground_sprites();
}

void PPU::draw_background()
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

        // Depends on adressing mode LCDC.4 (for BG & window)
        uint16_t tile_address = TILESET_START + 16 * tile_id + 2 * tile_offset_y;

        uint8_t tile_data_lsb = p_memory->read(tile_address);
        uint8_t tile_data_msb = p_memory->read(tile_address + 1);

        for (int tile_offset_x = 0; tile_offset_x < 8; ++tile_offset_x)
        {
            uint8_t x = tile_x * 8 + tile_offset_x - scx;
            uint8_t y = current_scanline - scy;

            uint8_t pixel_value = (tile_data_lsb & (1 << (7 - tile_offset_x))) >> (7 - tile_offset_x) | (tile_data_msb & (1 << (7 - tile_offset_x))) >> (7 - tile_offset_x) << 1;
            if (pixel_value != 0)
            {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

void PPU::draw_foreground_sprites()
{
    // Object attributes in $FE00-FE9F
    // 1 sprite = 4 bytes
    // Byte 0 = Y pos + 16
    uint8_t sprite_size_x = 8;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;

    uint8_t sprite_counter = 0;

    for (uint16_t sprite_address = OAM_START; sprite_address < 0x9F; sprite_address += 4)
    {
        uint8_t y_pos = this->p_memory->read(sprite_address);
        if (sprite_counter >= 10)
        {
            // Maximum 10 sprites per line
            break;
        }

        int highest_y = y_pos - 16 + sprite_size_y;
        int lowest_y = (y_pos + 16 + sprite_size_y);

        if (highest_y <= *p_LY || lowest_y >= *p_LY)
        {
            // sprite_size_y = 8, start drawing when y_pos is 8
            // Don't draw yet
            continue;
        }
        std::cout << *p_LY << ": drawing sprite " << sprite_counter << std::endl;
        this->draw_sprite(sprite_address);
        ++sprite_counter;
    }
}

void PPU::draw_sprite(uint16_t sprite_address)
{
    uint8_t x_pos, y_pos;
    uint8_t tile_index, sprite_attributes;

    y_pos = this->p_memory->read(sprite_address);
    x_pos = this->p_memory->read(sprite_address + 1);
    tile_index = this->p_memory->read(sprite_address + 2);
    sprite_attributes = this->p_memory->read(sprite_address + 3);

    uint8_t current_scanline = *p_LY;
    // uint8_t tile_y = current_scanline / 8;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;
    uint8_t sprite_size_x = 8;
    uint8_t tile_offset_y = current_scanline - y_pos * sprite_size_x;

    uint8_t sprite_byte_size = 16 * (this->obj_mode + 1);
    uint16_t tile_address = TILESET_START + tile_index * sprite_byte_size + 2 * tile_offset_y;

    uint8_t tile_data_lsb = p_memory->read(tile_address);
    uint8_t tile_data_msb = p_memory->read(tile_address + 1);

    for (int tile_offset_x = 0; tile_offset_x < 8; ++tile_offset_x)
    {
        uint8_t x = x_pos * 8 + tile_offset_x;
        uint8_t y = current_scanline;

        uint8_t pixel_value = ((tile_data_lsb >> (7 - tile_offset_x)) & 1) | ((tile_data_msb >> (7 - tile_offset_x)) & 1) << 1;
        if (pixel_value != 0)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

void PPU::wait_h()
{
    // 376 dots - mode3 length
    *p_LY = (*p_LY + 1) % 153;
}

void PPU::wait_v()
{
    // 4560 dots (10 scanlines)
}

void PPU::switch_to_next_mode()
{
    // std::cout << mode << " -> ";
    switch (mode)
    {
    case HORIZONTAL_BLANK:
        // wait_h();

        if (*p_LY == 143)
        {
            SDL_RenderPresent(renderer);

            this->cpu->request_interrupt(INT_VBLANK);

            mode = VERTICAL_BLANK;
        }
        else
        {
            (*p_LY)++;
            mode = OAM_SCAN;
        };
        break;
    case VERTICAL_BLANK:
        // wait_v();

        if (*p_LY == 153)
        {
            // std::cout << "Current FPS: " << std::to_string(1.0f / (2 * 16.666f / 1000)) << std::endl;
            // std::cout << "Cycles: " << std::to_string(cycles) << std::endl; // 4.19MHz

            last_frame_timestamp = SDL_GetPerformanceCounter();
            SDL_SetRenderDrawColor(renderer, 226, 243, 228, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            // SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

            float elapsed = (SDL_GetPerformanceCounter() - last_frame_timestamp) / (float)SDL_GetPerformanceFrequency();
            float elapsedMS = elapsed * 1000.0f;
            if (16.666f - elapsedMS > 0)
            {
                // Cap to 60 FPS
                // SDL_Delay(floor(16.666f - elapsedMS));
            }

            *p_LY = 0;
            mode = OAM_SCAN;
        }
        else
        {
            (*p_LY)++;
        }
        break;
    case OAM_SCAN:
        // OAM_scan();
        mode = DRAWING_PIXELS;
        break;
    case DRAWING_PIXELS:

        send_pixels();

        mode = HORIZONTAL_BLANK;
        break;
    };
}

void PPU::update(uint8_t cpu_cycles)
{
    uint8_t status = *p_LCDC;

    // status = 0; // Debug

    if (status & (1 << 7) == 0)
    {
        // Disabled for now
        return;
    }

    cycles_elapsed += cpu_cycles;
    // std::cout << std::endl
    //   << (int)cycles_elapsed << std::endl;
    // SDL_Delay(100);
    if (cycles_elapsed >= durations[mode])
    {
        cycles_elapsed -= durations[mode];
        switch_to_next_mode();
    }
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
