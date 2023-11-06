#include "opcodes.hpp"
#include <stdio.h>
#include <iostream>
#include "cpu.hpp"

#define CONST_ZERO_FLAG (1 << 7)
#define CONST_SUBSTRACT_FLAG (1 << 6)
#define CONST_HALFCARRY_FLAG (1 << 5)
#define CONST_CARRY_FLAG (1 << 4)

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

    if (x == 1)
    {
        // 8 bit loading
        // LD r[y], r[z]
        *cpu.registers[y] = *cpu.registers[z];
        return;
    }

    if (x == 0 && z == 1)
    {
        // LD rp[p], nn - (0x01, 0x11, 0x21, 0x31)
        // uint8_t q = y & (1 << 0);
        uint8_t p = (y & 6) >> 1;

        uint16_t value = arg2 << 8 | arg1; // inverted (lowest bit first)
        // uint16_t value = arg1 << 8 | arg2;
        std::cout << (int)p << " " << *cpu.registers16[p] << std::endl;
        *cpu.registers16[p] = value;
        return;
    }

    if (x == 0 && z == 6)
    {
        // LD nn, n (nn=B,C,D,E,H,L)
        *cpu.registers[y] = arg1;
        return;
    }

    uint16_t address;

    switch (opcode)
    {
    case 0x22:
        // LD (HLI), A
        // LD (HL+), A
        // LDI (HL), A
        cpu.m.rom[cpu.HL] = cpu.A;
        cpu.HL++;
        break;

    case 0x32:
        // LDD (HL), A
        // LDD (HL-), A
        // LD (HLD), A
        cpu.m.rom[cpu.HL] = cpu.A;
        cpu.HL--;

        // TODO: flags
        break;

    case 0xea:
        // LD (nn), A
        address = arg2 << 8 | arg1;
        cpu.m.rom[address] = cpu.A;
        break;
    case 0x2a:
        // LD A, (HL+)
        // 0010 1010
        // 00 101 010
        // x : 0, y: 2, z: 2
        // p: 2 q: 0

        cpu.A = cpu.m.rom[cpu.HL];
        cpu.HL++;
        break;

    case 0xe2:
        // LD ($FF00+C), A
        cpu.m.rom[0xFF00 + *cpu.p_C] = cpu.A;
        break;
    case 0x02:
        cpu.BC = cpu.A;
        break;

    default:
        std::cout << "UNK (LD)";
        std::cout << "xyz: " << std::hex << (int)x << (int)y << (int)z << std::endl;
        cpu.crash();
    }
}

void LDH(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t *p_in;
    uint8_t *p_out;

    if (opcode == 0xe0)
    { // y = 4
        // LDH (n), A
        p_in = &cpu.A;
        p_out = &cpu.m.rom[0xff00 + arg1];
    }
    else if (opcode == 0xf0)
    { // y = 6
        // LDH (n), A
        p_out = &cpu.A;
        p_in = &cpu.m.rom[0xff00 + arg1];
    }

    *p_out = *p_in;
}

void INC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    if (opcode == 0x2c)
    {
        (*cpu.p_L)++;
    }
}

void SUB(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    if (opcode == 0x90)
    {
        (*cpu.p_B)--;
    }
}

void SBC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    if (y == 3)
    {
        // SBC A,n
        cpu.A -= (uint8_t)(*cpu.registers[z]);
        cpu.A -= (cpu.flags & (1 << 5) >> 5);

        // TODO : improve flag handling, incrorrect
        cpu.flags = cpu.flags | (~(cpu.A == 0) << 8); // Set if result is zero
        cpu.flags = cpu.flags | (1 << 7);             // Set.

        if ((int)(x & 0xf) - (int)(y & 0xf) < 0)
        {
            cpu.flags = cpu.flags | (1 << 6);
        }
    }
}

void DEC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    // uint8_t q = y & (1 << 0);
    uint8_t p = (y & 6) >> 1;

    bool zero = 0;
    if (z == 5)
    {
        // 8-bit dec
        (*cpu.registers[y])--;
        if (*cpu.registers[y] == 0)
        {
            zero = 1;
        };
    }
    else if (z == 3)
    {
        // If DEC then q is automatically 1
        (*cpu.registers16[p])--;
        if (*cpu.registers16[p] == 0)
        {
            zero = 1;
        };
    }
    cpu.flags = cpu.flags | CONST_ZERO_FLAG; // Z - set if result is zero
    // C - not affected
    // N - set
    cpu.flags |= CONST_SUBSTRACT_FLAG;
    // H - set if borrow
}

void JR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    int8_t offset = arg1;
    // std::cout << std::dec << i-2 << std::endl;
    if (y == 3)
    {
        // JR n
        cpu.PC += offset;
        return;
    }
    // JR cc, n

    // z = cpu.flags;
    // std::cout << "(flags " << std::hex << (int)cpu.flags << ")" << std::endl;
    // std::cout << "(const " << std::hex << (int)CONST_ZERO_FLAG << ")" << std::endl;
    // std::cout << "(const " << std::hex << (int)(cpu.flags & CONST_ZERO_FLAG) << ")" << std::endl;
    switch (y - 4)
    {
    case 0:
        if (!(cpu.flags & CONST_ZERO_FLAG))
        {
            cpu.PC += offset;
        }
        break;
    case 1:
        if ((cpu.flags & CONST_ZERO_FLAG))
        {
            cpu.PC += offset;
        }
        break;
    case 2:
        if (!(cpu.flags & CONST_CARRY_FLAG))
        {
            cpu.PC += offset;
        }
        break;
    case 3:
        if ((cpu.flags & CONST_CARRY_FLAG))
        {
            cpu.PC += offset;
        }
        break;
    };
}

void CPL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.A = ~cpu.A;
}

void CALL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // uint16_t addr = arg1 << 8 | arg2;
    uint16_t value = cpu.PC + 1;
    cpu.push(value);
    JP(cpu, arg1, arg2);
}

// ALU

void XOR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    if (x == 2)
    {
        cpu.A = (*cpu.registers[z]) ^ cpu.A;
    }
    else
    {
        cpu.A = arg1 ^ cpu.A;
    }
}

void OR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    if (opcode == 0xf6)
    {
        cpu.A |= arg1;
    }
    cpu.A |= *cpu.registers[z];

    // Set
    if (cpu.A == 0)
    {
        cpu.flags |= CONST_ZERO_FLAG;
    }
    // Reset
    cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    cpu.flags &= ~CONST_HALFCARRY_FLAG;
    cpu.flags &= ~CONST_CARRY_FLAG;
}

void DI(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.disable_interrupts_next = true;
    // Disable interrupts after next instruction has been executed
}

void EI(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.disable_interrupts_next = false;
    // Enables interrupts after next instruction has been executed
}

void CP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);
    // x = 2, y=7
    uint8_t *p_reg = cpu.registers[z];

    if (cpu.A == *p_reg)
    {
        cpu.flags |= CONST_ZERO_FLAG;
    }
    if (cpu.A < *p_reg)
    {
        cpu.flags |= CONST_CARRY_FLAG;
    }

    cpu.flags |= CONST_SUBSTRACT_FLAG;
    // H set if no borrow from bit 4
    // ...
}

void RET(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    bool ret = false;
    if (z == 0)
    {
        // RET cc - Conditional return
        switch (y)
        {
        case 0:
            ret = !(cpu.flags & CONST_ZERO_FLAG);
            break;
        case 1:
            ret = (cpu.flags & CONST_ZERO_FLAG);
            break;
        case 2:
            ret = !(cpu.flags & CONST_CARRY_FLAG);
            break;
        case 3:
            ret = (cpu.flags & CONST_CARRY_FLAG);
            break;
        };
    }
    if (z == 1 || ret)
    {
        // RET
        uint16_t address = cpu.pop();
        cpu.PC = address;
    }
}

void RST(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // Restart
    uint8_t opcode = cpu.m.rom[cpu.PC + 1];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    cpu.push(cpu.PC);
    cpu.PC = y * 8 - 1;

    if (y * 8 == 0x38)
    {
        std::cout << "Reached RST 38H, there must be a mistake" << std::endl;
        cpu.crash();
    }
}

// CB-prefixed

void RES(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.rom[cpu.PC + 1];
    uint8_t x, y, z;
    cpu.extract_grouping(opcode, x, y, z);

    uint8_t *p_reg = cpu.registers[z];

    // Reset bit y in register z
    *p_reg &= ~(1 << y);
}