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
    // p_memory->write(LY, 0x90); // Debug mode

    p_SCX = p_memory->get_pointer(SCX);
    p_SCY = p_memory->get_pointer(SCY);

    p_color_palette = p_memory->get_pointer(COLOR_PALETTE);

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
    // std::cout.clear();
    // std::cout << "obj mode: " << (int)obj_mode << std::endl;
    // std::cout << "obj enable: " << (int)draw_fg << std::endl;
    // std::cout << std::dec << std::endl
    //           << "OAM scan, LY=" << (int)(*p_LY) << std::endl;

    for (uint16_t sprite_address = OAM_START; sprite_address < 0xFE9F; sprite_address += 4)
    {
        if (num_sprites >= 10) // Maximum 10 sprites per line
            break;

        uint8_t y_pos = this->p_memory->read(sprite_address);

        // std::cout << std::dec <<    "y_pos=" << (int)y_pos << std::endl;

        uint8_t highest_y = y_pos;
        uint8_t lowest_y = y_pos + sprite_size_y;

        if (highest_y > *p_LY + (uint8_t)16 || lowest_y < *p_LY + (uint8_t)16)
            continue;

        sprites_adresses[num_sprites++] = sprite_address;
    }

    // std::cout.setstate(std::ios_base::failbit);

    // std::cin.ignore();
}

void PPU::send_pixels()
{
    // Send pixels to LCD, video memory is locked (172 dots -> 289 dots)

    draw_bg_win = (*p_LCDC >> 0) & 1;

    draw_fg = (*p_LCDC >> 1) & 1;

    obj_mode = (*p_LCDC >> 2) & 1; // 0 or 1;

    // Background
    // draw_bg_win = false;
    if (draw_bg_win)
        this->draw_background_tiles();
    draw_fg = false;
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

            // uint8_t pixel_value = (tile_data_lsb & (1 << (7 - tile_offset_x))) >> (7 - tile_offset_x) | (tile_data_msb & (1 << (7 - tile_offset_x))) >> (7 - tile_offset_x) << 1;
            uint8_t pixel_value_id = ((tile_data_lsb >> (7 - tile_offset_x)) & 1) | ((tile_data_msb >> (7 - tile_offset_x)) & 1) << 1;
            // ID is 0-3
            uint8_t color = (*p_color_palette >> 2 * pixel_value_id) & 3;

            if (color != 0)
            {
                switch (color)
                {
                case 1:
                    // Light gray
                    SDL_SetRenderDrawColor(renderer, 0x88, 0xC0, 0x70, SDL_ALPHA_OPAQUE);
                    break;
                case 2:
                    // Dark gray
                    SDL_SetRenderDrawColor(renderer, 0x34, 0x68, 0x56, SDL_ALPHA_OPAQUE);
                    break;
                case 3:
                    // Black
                    SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, SDL_ALPHA_OPAQUE);
                    break;
                }
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

void PPU::draw_all_sprites()
{
    uint8_t x_pos = 0;
    uint8_t y_pos = 0;
    uint8_t xsize = 8;
    uint8_t ysize = 16;
    uint8_t tile_index = 0;
    uint8_t current_scanline = *p_LY;

    uint8_t tile_row = *p_LY / ysize;
    // if (*p_LY > ysize)
    //     return;
    for (uint16_t sprite_address = OAM_START; sprite_address < 0xFE9F; sprite_address += 4)
    {
        // 40 times
        // 160x144 -> 20 x 18 8x8 objects
        uint8_t tile_offset_y = current_scanline - y_pos * ysize;
        uint16_t tile_address = TILESET_START + tile_row * (20 - 1) * 2 * 16 + tile_index * 16 + 2 * tile_offset_y;
        draw_tile(tile_address, x_pos, *p_LY);

        tile_index++;
        x_pos += 8;
        // y_pos += 8;

        if (x_pos >= 160)
        {
            y_pos += 8 * (x_pos / 160);
            x_pos = x_pos % 160;
            break;
        }
    }
}

void PPU::draw_foreground_sprites()
{
    // draw_all_sprites();
    // return;

    // std::cout << "drawing foreground" << std::endl;
    // Object attributes in $FE00-FE9F
    // 1 sprite = 4 bytes => 40 sprites on screen
    // Byte 0 = Y pos + 16

    // Sprite size 8x8 or 8x16
    uint8_t sprite_size_x = 8;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;

    uint8_t current_scanline = *p_LY;

    std::cout.clear();
    std::cout << std::dec << "LY=" << (int)(*p_LY) << ", drawing " << (int)num_sprites << " sprites" << std::endl;
    std::cout.setstate(std::ios_base::failbit);

    for (uint8_t i = 0; i < num_sprites; ++i)
    {
        uint16_t sprite_address = sprites_adresses[i];

        uint8_t y_pos = p_memory->read(sprite_address);
        uint8_t x_pos = p_memory->read(sprite_address + 1);

        draw_sprite(sprite_address, x_pos, y_pos);
    }

    // break;
}

// if (*p_LY > 0)
// std::cout << "Line " << (int)*p_LY << " : Drawing " << (int)sprite_counter << " objects" << std::endl;
// }

void PPU::draw_tile(uint16_t tile_address, uint8_t x_pos, uint8_t y_pos)
{

    uint8_t tile_data_lsb = p_memory->read(tile_address);
    uint8_t tile_data_msb = p_memory->read(tile_address + 1);
    // uint8_t current_scanline = *p_LY;

    for (int tile_offset_x = 0; tile_offset_x < 8; ++tile_offset_x)
    {
        uint8_t x = x_pos + tile_offset_x;
        // uint8_t y = current_scanline;

        uint8_t pixel_value = ((tile_data_lsb >> (7 - tile_offset_x)) & 1) | ((tile_data_msb >> (7 - tile_offset_x)) & 1) << 1;
        if (pixel_value != 0)
        {
            SDL_RenderDrawPoint(renderer, x, y_pos);
        }
    }
}

void PPU::draw_sprite(uint16_t sprite_address, uint8_t x_pos, uint8_t y_pos)
{
    uint8_t tile_index, sprite_attributes;

    tile_index = this->p_memory->read(sprite_address + 2);
    sprite_attributes = this->p_memory->read(sprite_address + 3);

    std::cout.clear();
    std::cout << std::hex << "drawing sprite : 0x" << sprite_address << std::dec << " (" << (int)x_pos << ", " << (int)y_pos << "), " << (int)tile_index << std::endl;

    uint8_t current_scanline = *p_LY;
    // uint8_t tile_y = current_scanline / 8;
    uint8_t sprite_size_y = obj_mode ? 16 : 8;
    uint8_t sprite_size_x = 8;
    uint8_t tile_offset_y = current_scanline - y_pos;

    uint8_t sprite_byte_size = 16 * (this->obj_mode + 1);

    // uint8_t tile_offset_y = current_scanline - y_pos * ysize;
    // uint16_t tile_address = TILESET_START + tile_index * 16 + 2 * tile_offset_y;

    uint16_t tile_address = TILESET_START + tile_index * 16 + 2 * tile_offset_y;

    this->draw_tile(tile_address, x_pos, y_pos + tile_offset_y);
    std::cout.setstate(std::ios_base::failbit);
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

            cpu->request_interrupt(INT_VBLANK);

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

        if (*p_LY == 154) // 153?
        {
            // std::cout << "Current FPS: " << std::to_string(1.0f / (2 * 16.666f / 1000)) << std::endl;
            // std::cout << "Cycles: " << std::to_string(cycles) << std::endl; // 4.19MHz

            last_frame_timestamp = SDL_GetPerformanceCounter();
            SDL_SetRenderDrawColor(renderer, 0xE0, 0xF8, 0xD0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            // SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0x08, 0x18, 0x20, SDL_ALPHA_OPAQUE);

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
    uint8_t status = *p_LCDC;

    // status = 0; // Debug

    if ((status & (1 << 7)) == 0)
    {
        // Disabled for now
        std::cout.clear();
        std::cout << "ppu disabled!" << std::endl;
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
