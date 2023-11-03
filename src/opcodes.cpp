#include "opcodes.hpp"
#include <stdio.h>
#include <iostream>
#include "cpu.hpp"

void NOP(CPU &cpu, uint8_t arg1, uint8_t arg2){
    // cpu.PC += 1;
};

void UNK(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // Placeholder for all function that have not yet been implemented
    std::cout << "Not Implemented" << std::endl;
    exit(1);
}

void JP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // uint16_t address = arg1 << 8 | arg2;
    uint16_t address = arg2 << 8 | arg1;
    cpu.PC = address - 3;
}

void LD(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    if (x=1){
        // 8 bit loading
        // LD r[y], r[z]
        cpu.registers[y] = cpu.registers[z];
        return;
    }

    if (opcode == 0x11){
        // LD DE, nn
        // std::cout << std::hex << (int)arg1 << ";" << (int)arg2<< std::endl;
        uint16_t value = arg1 << 8 | arg2;
        // std::cout << std::hex << (int)value;
        *cpu.p_D = arg1;
        *cpu.p_E = arg2;
    }
    else if (opcode == 0x22){
        // LD (HLI), A
        // LD (HL+), A
        // LDI (HL), A
        cpu.HL = cpu.A + 1;
        // std::cout << std::hex << (int)opcode;
    }
    else{
        std::cout << "UNK (LD)";
    }
}

void INC(CPU &cpu, uint8_t arg1, uint8_t arg2){
    uint8_t opcode = cpu.m.rom[cpu.PC];
    if (opcode == 0x2c){
        *cpu.p_L++;
    }
}

void SUB(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    if (opcode == 0x90){
        (*cpu.p_B)--;
    }
}

void SBC(CPU &cpu, uint8_t arg1, uint8_t arg2){
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    if(y==3){
        // SBC A,n
        cpu.A -= (uint8_t)(*cpu.registers[z]);
        cpu.A -= (cpu.flags & (1 << 5) >> 5);

        // TODO : improve flag handling, incrorrect
        cpu.flags = cpu.flags | (~(cpu.A == 0) << 8); //Set if result is zero
        cpu.flags = cpu.flags | (1 << 7); // Set.
        
        if ((int)(x & 0xf) - (int)(y & 0xf) < 0){
            cpu.flags = cpu.flags | (1 << 6);
        }
    }
}

void DEC(CPU &cpu, uint8_t arg1, uint8_t arg2){
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    uint8_t q = y & (1 << 0);
    uint8_t p = (y & 6) >> 1;

    if (z==5){
        // 8-bit dec
        cpu.registers[y]--;
    } else if (z==3 and q==1){
        //If DEC then q is automatically 1
        cpu.registers16[p]--;
    }
}

void JR(CPU &cpu, uint8_t arg1, uint8_t arg2){
    cpu.PC += arg1 - 2;
}

void CPL(CPU &cpu, uint8_t arg1, uint8_t arg2){
    cpu.A = ~cpu.A;
}

void CALL(CPU &cpu, uint8_t arg1, uint8_t arg2){
    // uint16_t addr = arg1 << 8 | arg2;
    uint16_t value = cpu.PC+1;
    cpu.push(value);
    JP(cpu, arg1, arg2);
}

void XOR(CPU &cpu, uint8_t arg1, uint8_t arg2){
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    if (x==2){
        cpu.A = (*cpu.registers[z]) ^ cpu.A;
    } else {
        cpu.A = arg1 ^ cpu.A;
    }
}

