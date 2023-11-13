#include "ppu.hpp"
#include <stdio.h>
#include <iostream>
#include <bitset>

/**
 * PPU - Pixel Processing Unit
 */

PPU::PPU(MemoryController *p_m, SDL_Renderer *renderer)
{
    set_memory_controller(p_m);
    this->renderer = renderer;

    p_LCDC = p_memory->get_pointer(LCDC);
    p_LY = p_memory->get_pointer(LY);
    // p_memory->write((uint8_t)LY, (uint8_t)0x90); // Debug mode
    
    p_SCX = p_memory->get_pointer(SCX);
    p_SCY = p_memory->get_pointer(SCY);
    
    p_colPalette = p_memory->get_pointer(COLOR_PALETTE);
    pf = new PixelFetcher();
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

    // Background
    uint8_t current_scanline = *p_LY;
    uint8_t tile_y = current_scanline / 8;
    uint8_t tile_offset_y = current_scanline - tile_y * 8;

    //Scroll values
    uint8_t scx, scy;
    scx = *p_SCX;
    scy = *p_SCY;

    for (int tile_x = 0; tile_x < 32; tile_x++)
    {
        uint8_t tile_id = p_memory->read(TILEMAP1_START + 32 * tile_y + tile_x);

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
    // SDL_RenderPresent(renderer); // Draw each scanline - TO REMOVE later
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
            // SDL_Delay(160);
            SDL_SetRenderDrawColor(renderer, 226, 243, 228, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            // SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

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

    if (status & (1 << 7) == 0){
        // Disabled for now
        return;
    }

    cycles_elapsed += cpu_cycles;
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

/** Pixel Fetcher **/
void PixelFetcher::get_tile()
{
    // x = ((SCX / 8) + x) & $1F;
    // y = (y + SCY) & 255;
}

void PixelFetcher::get_data()
{
}

void PixelFetcher::clear_queues()
{
    obj_FIFO.clear();
    bgd_FIFO.clear();
}
