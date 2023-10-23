#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <stdint.h>

class cpu{

    
    uint8_t A, B, C, D, E, F;
    uint16_t HL;
    uint8_t H = HL;
    uint8_t L = HL+1;
    uint8_t flag;
    
    uint16_t SP;
    uint16_t PC = 256; // 0x100

    public:
        uint8_t decode(uint8_t opcode); 
};

#endif