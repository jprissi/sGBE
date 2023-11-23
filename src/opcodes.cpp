#include <stdio.h>
#include <iostream>
#include <bitset>

#include "opcodes.hpp"
#include <assert.h>
#include "cpu.hpp"

// http://www.z80.info/decoding.htm
uint8_t x, y, z;

void NOP(CPU &cpu, uint8_t arg1, uint8_t arg2){
    // Easy, do nothing!
};

void HALT(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.halt = true;
    cpu.PC--;
}

void UNK(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // Placeholder for all function that have not yet been implemented
    std::cout << "Not Implemented" << std::endl;
    exit(1);
}

void LD(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    if (x == 1)
    {
        // 8 bit loading
        // LD r[y], r[z]
        if (y == 6)
        {
            // LD r[y], (HL) - WRONG
            // LD (HL), r[z]

            // *cpu.registers[y] = cpu.m.read(cpu.HL);
            cpu.m.write(cpu.HL, *cpu.registers[z]);
        }
        else if (z == 6)
        {
            *cpu.registers[y] = cpu.m.read(cpu.HL);
        }
        else
        {
            *cpu.registers[y] = *cpu.registers[z];
        }
        return;
    }

    if (x == 0 && z == 1)
    {
        // LD rp[p], nn - (0x01, 0x11, 0x21, 0x31)
        // uint8_t q = y & (1 << 0);
        uint8_t p = (y & 6) >> 1;

        uint16_t value = arg2 << 8 | arg1; // inverted (lowest bit first)
        // uint16_t value = arg1 << 8 | arg2;
        // std::cout << (int)p << " " << *cpu.registers16[p] << std::endl;
        *cpu.registers16[p] = value;
        return;
    }

    if (x == 0 && z == 6)
    {
        // LD nn, n (nn=B,C,D,E,H,L)
        // 0x36, ...
        *cpu.get_register(y) = arg1;
        return;
    }

    uint16_t address;

    switch (opcode)
    {
    case 0x02:
        cpu.BC = *cpu.p_A;
        break;
    case 0x08:
        // LD (nn), SP
        address = arg1 | arg2 << 8;
        cpu.m.write(address, cpu.SP & 0xFF);
        cpu.m.write(address + 1, (cpu.SP & 0xFF00) >> 8);
        break;
    case 0x0a:
        // LD A, (BC)
        *cpu.p_A = cpu.m.read(cpu.BC);
        break;
    case 0x12:
        // LD (DE), A
        address = *cpu.registers16[1]; // DE
        cpu.m.write(address, *cpu.p_A);
        break;
    case 0x1a:
        // LD A, (DE)
        *cpu.p_A = cpu.m.read(cpu.DE);
        break;
    case 0x22:
        // LD (HLI), A
        // LD (HL+), A
        // LDI (HL), A
        cpu.m.write(cpu.HL, *cpu.p_A);
        cpu.HL++;
        break;

    case 0x2a:
        // LD A, (HL+)
        // 0010 1010
        // 00 101 010
        // x : 0, y: 2, z: 2
        // p: 2 q: 0

        *cpu.p_A = cpu.m.read(cpu.HL);
        cpu.HL += 1;
        // cpu.step_by_step = true;
        break;
    case 0x32:
        // LDD (HL), A
        // LDD (HL-), A
        // LD (HLD), A
        cpu.m.write(cpu.HL, *cpu.p_A);
        cpu.HL--;

        // TODO: flags
        break;
    case 0x3a:
        // LDD A, (HL) and all its 2 other variations
        *cpu.p_A = cpu.m.read(cpu.HL);
        cpu.HL--;
        break;

    case 0x7a:
        // LD A, (HL)
        *cpu.p_A = cpu.m.read(cpu.HL);

        break;
    case 0xe2:
        // LD ($FF00+C), A
        cpu.m.write(0xFF00 + *cpu.p_C, *cpu.p_A);
        break;
    case 0xea:
        // LD (nn), A
        address = arg2 << 8 | arg1;
        cpu.m.write(address, *cpu.p_A);
        break;

    case 0xf2:
        // LD A, ($FF00+C)
        *cpu.p_A = cpu.m.read(0xFF00 + *cpu.p_C);
        break;
    case 0xf8:

        cpu.HL = cpu.SP + (int8_t)arg1;

        *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        if ((cpu.SP & 0x0F) + ((int8_t)arg1 & 0x0F) > 0x0F)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }

        *cpu.flags &= ~CONST_CARRY_FLAG;
        if ((uint16_t)((uint8_t)(cpu.SP & 0xFF) + arg1) > 0xFF) // Not straightforward, carry flag is the result of unsigned 8 bit addition (https://www.reddit.com/r/EmuDev/comments/y51i1c/game_boy_dealing_with_carry_flags_when_handling/)
        {
            *cpu.flags |= CONST_CARRY_FLAG;
        }

        // cpu.SP += (int8_t)arg1; // Signed immediate value

        *cpu.flags &= ~CONST_ZERO_FLAG;
        *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
        break;
    case 0xf9:
        // LD SP, HL or [SP] ?
        cpu.SP = cpu.HL;
        break;
    case 0xfa:
        // LD A, (nn)
        address = arg2 << 8 | arg1;
        // address = arg1 << 8 | arg2;
        *cpu.p_A = cpu.m.read(address);
        break;

    default:
        std::cout << " UNK (LD)";
        std::cout << "xyz: " << std::hex << (int)x << (int)y << (int)z << std::endl;
        cpu.panic();
    }
}

void LDH(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    uint8_t *p_in;
    // uint8_t *p_out;
    uint16_t address_out;

    if (opcode == 0xe0)
    { // y = 4
        // LDH (n), A
        // p_in = cpu.p_A);
        address_out = (0xff00 | arg1);
        cpu.m.write(address_out, *cpu.p_A);
    }
    else if (opcode == 0xf0)
    { // y = 6
        // LDH A, (0xff+n)
        *cpu.p_A = cpu.m.read((0xff00 | arg1));
    }
    else
    {
        std::cout << "UNK (LDH)" << std::endl;
        exit(1);
    }
}

void ADD16(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t p = (y & 6) >> 1;

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;

    if ((cpu.HL & 0xFFF) + (*cpu.registers16[p] & 0xFFF) > 0xFFF)
    {
        *cpu.flags |= CONST_HALFCARRY_FLAG;
    }
    if ((uint32_t)cpu.HL + (uint32_t)*cpu.registers16[p] > 0xFFFF)
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    cpu.HL += *cpu.registers16[p];
}

void ADD8(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t value;

    if (opcode == 0xC6)
    {
        value = arg1;
    }
    else
    {
        value = *cpu.get_register(z);
    }

    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    if ((*cpu.p_A & 0x0F) + (value & 0x0F) > 0x0F)
    {
        *cpu.flags |= CONST_HALFCARRY_FLAG;
    }

    *cpu.flags &= ~CONST_CARRY_FLAG;
    if ((uint16_t)(*cpu.p_A + value) > 0xFF)
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    (*cpu.p_A) += value;

    *cpu.flags &= ~CONST_ZERO_FLAG;
    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
}
void ADD(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    if (opcode == 0xe8)
    {
        *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        if ((cpu.SP & 0x0F) + ((int8_t)arg1 & 0x0F) > 0x0F)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }

        *cpu.flags &= ~CONST_CARRY_FLAG;
        if ((uint16_t)((uint8_t)(cpu.SP & 0xFF) + arg1) > 0xFF) // Not straightforward, carry flag is the result of unsigned 8 bit addition (https://www.reddit.com/r/EmuDev/comments/y51i1c/game_boy_dealing_with_carry_flags_when_handling/)
        {
            *cpu.flags |= CONST_CARRY_FLAG;
        }
        cpu.SP += (int8_t)arg1; // Signed immediate value

        *cpu.flags &= ~CONST_ZERO_FLAG;
        *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
        return;
    }
    if (z == 1 && x == 0)
    {
        ADD16(cpu, arg1, arg2);
    }
    else if (x == 2 | opcode == 0xc6)
    {
        ADD8(cpu, arg1, arg2);
    }
    else
    {
        std::cout << "UNK (ADD) " << std::endl;
        exit(1);
    }
}

void ADC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{

    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    uint8_t value;
    if (opcode == 0xce)
    {
        value = arg1;
    }
    else
    {
        value = *cpu.get_register(z);
    }

    // std::cout << std::endl << std::hex << (int)value << std::endl;
    // std::cout << std::bitset<8>(*cpu.flags) << std::endl;

    uint8_t carry = (*cpu.flags & CONST_CARRY_FLAG) >> 4;
    // value += carry;

    // std::cout << std::hex << (int)((*cpu.flags >> 4) & 1) << std::endl;
    // std::cout << std::hex << (int)value << std::endl;
    // cpu.step_by_step = true;

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
    *cpu.flags &= ~CONST_ZERO_FLAG;

    if ((uint16_t)(*cpu.p_A + value + carry) > 0xFF)
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    if ((*cpu.p_A & 0x0F) + (value & 0x0F) + carry > 0x0F)
    {
        *cpu.flags |= CONST_HALFCARRY_FLAG;
    }

    (*cpu.p_A) += value + carry;

    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void SUB(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;

    *cpu.flags |= CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
    *cpu.flags &= ~CONST_ZERO_FLAG;

    if (opcode == 0x90)
    {
        if (*cpu.p_A < *cpu.p_B)
        {
            *cpu.flags |= CONST_CARRY_FLAG;
        }
        if ((int)(*cpu.p_A & 0x0F) - (int)(*cpu.p_B & 0x0F) < 0)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }
        (*cpu.p_A) -= (*cpu.p_B);
        if ((*cpu.p_A) == 0)
        {
            *cpu.flags |= CONST_ZERO_FLAG;
        }
    }
    else if (opcode == 0xd6)
    {
        // cpu.step_by_step = true;
        // SUB d8
        if (*cpu.p_A < arg1)
        {
            *cpu.flags |= CONST_CARRY_FLAG;
        }
        // if ((*cpu.p_A & 0x0F) < arg1)
        // {
        //     *cpu.flags |= CONST_HALFCARRY_FLAG;
        // }
        if ((int)(*cpu.p_A & 0xf) - (int)(arg1 & 0xf) < 0)
        {
            *cpu.flags = *cpu.flags | CONST_HALFCARRY_FLAG;
        }

        *cpu.p_A -= arg1;
        if ((*cpu.p_A) == 0)
        {
            *cpu.flags |= CONST_ZERO_FLAG;
        }
    }
    else
    {
        std::cout << "UNK (ADD) " << std::endl;
        exit(1);
    }
}

void SBC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    if (y == 3)
    {
        uint8_t carry = (*cpu.flags & CONST_CARRY_FLAG) >> 4;
        uint8_t value;

        if (opcode == 0xDE)
        {
            value = arg1;
        }
        else
        {
            value = *cpu.get_register(z);
        }

        *cpu.flags &= ~CONST_CARRY_FLAG;
        if ((uint16_t)(*cpu.p_A - value - carry < 0))
        {
            *cpu.flags |= CONST_CARRY_FLAG;
        }
        *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        if ((int)(*cpu.p_A & 0xf) - (int)(value & 0xf) - (int)carry < 0)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }

        // SBC A,n
        *cpu.p_A -= (value + carry);
        *cpu.flags &= ~CONST_ZERO_FLAG;

        if (*cpu.p_A == 0)
        {
            *cpu.flags |= CONST_ZERO_FLAG;
        }

        *cpu.flags |= CONST_SUBSTRACT_FLAG;
    }
    else
    {
        std::cout << "UNK (SBC) " << std::endl;
        exit(1);
    }
}

void INC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t p = (y & 6) >> 1;

    if (z == 4)
    {
        *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
        // 8 bit inc

        *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        if ((*cpu.registers[y] & 0x0f) + 1 > 0x0f)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }

        (*cpu.registers[y])++;

        *cpu.flags &= ~CONST_ZERO_FLAG;
        if (*cpu.registers[y] == 0)
        {
            *cpu.flags |= CONST_ZERO_FLAG;
        }

        return;
    }
    else if (z == 3)
    {
        // No effect on flags according to manual

        (*cpu.registers16[p])++;

        return;
    }
    std::cout << "Not implemented (INC)" << std::endl;
    cpu.panic();
}

void DEC(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    // uint8_t q = y & (1 << 0);
    uint8_t p = (y & 6) >> 1;

    bool zero = false;

    if (z == 5)
    {
        // 8-bit dec
        uint8_t *p_reg = cpu.get_register(y);
        if ((*p_reg & 0x0f) == 0x00)
        {
            *cpu.flags |= CONST_HALFCARRY_FLAG;
        }
        else
        {
            *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        }

        (*p_reg)--;
        if (*p_reg == 0)
        {
            zero = 1;
        };
    }
    else if (z == 3)
    {
        // DEC nn (no flag affected)
        // If DEC then q is automatically 1
        // if ((*cpu.registers16[p] & 0x0f) == 0x00)
        // {
        //     *cpu.flags |= CONST_HALFCARRY_FLAG;
        // }
        // else
        // {
        //     *cpu.flags &= ~CONST_HALFCARRY_FLAG;
        // }

        (*cpu.registers16[p])--;

        return; // (no flags affected)
    }
    else
    {
        std::cout << "UNK (DEC) " << std::endl;
        exit(1);
    }

    *cpu.flags &= ~CONST_ZERO_FLAG;
    if (zero)
    {
        *cpu.flags |= CONST_ZERO_FLAG; // Z - set if result is zero
    }
    // C - not affected
    // N - set
    *cpu.flags |= CONST_SUBSTRACT_FLAG;
    // H - set if borrow
}

// JUMPS

void JP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    uint16_t address;
    if (opcode == 0xe9)
    {
        // address = cpu.HL + 2;
        address = cpu.HL;
    }
    else
    {
        address = arg2 << 8 | arg1;
    }

    if (x == 3 and z == 2)
    {
        // JP cc, nn
        switch (y)
        {
        case 0:
            if (!(*cpu.flags & CONST_ZERO_FLAG))
            {
                // cpu.PC += offset;
                cpu.PC = address - cpu.next_instruction_relative_pos;
            }
            break;
        case 1:
            if ((*cpu.flags & CONST_ZERO_FLAG))
            {
                cpu.PC = address - cpu.next_instruction_relative_pos;
            }
            break;
        case 2:
            if (!(*cpu.flags & CONST_CARRY_FLAG))
            {
                cpu.PC = address - cpu.next_instruction_relative_pos;
            }
            break;
        case 3:
            if ((*cpu.flags & CONST_CARRY_FLAG))
            {
                cpu.PC = address - cpu.next_instruction_relative_pos;
            }
            break;
        };
    }
    else
    {
        cpu.PC = address - cpu.next_instruction_relative_pos; // = address - 3 (1 byte for jmp + 2 bytes for address)
    }
}

void JR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    int8_t offset = (int8_t)arg1;
    // std::cout << std::dec << i-2 << std::endl;

    // std::cout << std::endl << std::dec << (int)offset<< std::endl;
    // cpu.print_registers();

    if (y == 3)
    {
        // JR n
        // std::cout << opcode << std::endl;
        cpu.PC += offset; //+ 2; //- cpu.next_instruction_relative_pos;
        // std::cout << std::dec <<  offset << std::endl;
        // cpu.print_registers();
        // std::cin.ignore();
        return;
    }
    // // JR cc, n
    // std::cout << std::hex << (int)y << std::endl;
    // std::cout << ((*cpu.flags >> 7) & 1) << std::endl;
    // std::cout << ((*cpu.flags >> 4) & 1) << std::endl;
    // std::cout << (bool)((*cpu.flags & CONST_ZERO_FLAG)) << std::endl;
    switch (y - 4)
    {
    case 0:
        if (!((*cpu.flags >> 7) & 1))
        {
            cpu.PC += offset;
            std::cout << "NZ";
        }
        return;
        break;
    case 1:
        if ((*cpu.flags >> 7) & 1)
        {
            cpu.PC += offset;
            std::cout << "Z";
            // std::cin.ignore();
        }
        return;
        break;
    case 2:
        if (!((*cpu.flags >> 4) & 1))
        {
            cpu.PC += offset;
            std::cout << "NC";
            return;
        }
        return;
        break;
    case 3:
        if ((*cpu.flags & CONST_CARRY_FLAG))
        {
            cpu.PC += offset;
            std::cout << "C";
        }
        break;
    };

    // cpu.print_registers();
    // std::cin.ignore();
}

// ALU

void AND(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t value;
    if (opcode == 0xe6)
    {
        value = arg1;
    }
    else if (opcode == 0xa6)
    {
        value = cpu.m.read(cpu.HL);
    }
    else
    {
        value = *cpu.registers[z];
    }

    (*cpu.p_A) &= value;

    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags |= CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
}

void XOR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{

    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    if (x == 2)
    {
        // alu[y] r[z]
        *cpu.p_A ^= (*cpu.get_register(z)); //^ *cpu.p_A;
        // Logical XOR (and not bitwise)
        // *cpu.p_A = !(*cpu.get_register(z)) != !(*cpu.p_A);
    }
    else if (x == 3)
    {
        // alu[y] n
        *cpu.p_A = arg1 ^ *cpu.p_A;
        // *cpu.p_A = !(arg1) != !(*cpu.p_A);
    }
    else
    {
        std::cout << "UNK (XOR)" << std::endl;
    }

    *cpu.flags &= ~CONST_ZERO_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;

    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void OR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    if (opcode == 0xf6)
    {
        *cpu.p_A |= arg1;
    }
    else
    {
        *cpu.p_A |= *cpu.get_register(z);
    }
    // Set
    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }
    // Reset
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
}

void DI(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.disable_interrupts_next = true;
    // Disable interrupts after next instruction has been executed
}

void EI(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.enable_interrupts_next = true;
    // cpu.step_by_step=true;
    // Enables interrupts after next instruction has been executed
}

void CP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.current_opcode;
    // x = 2, y=7

    uint8_t value;
    if (opcode == 0xfe)
    {
        value = arg1;
    }
    else if (opcode == 0xbe)
    {
        value = cpu.m.read(cpu.HL);
    }
    else
    {

        cpu.compute_current_opcode_groupings(x, y, z);
        value = *cpu.registers[z];
    }

    std::cout << "\t => " << (int)value << " <=? " << std::hex << (int)(*cpu.p_A);

    if (*cpu.p_A == value)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }
    if (*cpu.p_A < value)
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_CARRY_FLAG;
    }

    *cpu.flags |= CONST_SUBSTRACT_FLAG;

    // H set if no borrow from bit 4
    if ((int)(*cpu.p_A & 0x0F) - (int)(value & 0x0F) < 0)
    {
        *cpu.flags |= CONST_HALFCARRY_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    }
}

void CPL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    *cpu.p_A = ~*cpu.p_A;
}

void CALL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // uint16_t addr = arg1 << 8 | arg2;
    uint8_t opcode = cpu.current_opcode;

    bool cc = false;

    if (opcode != 0xcd)
    {
        // call cc, nn

        cpu.compute_current_opcode_groupings(x, y, z);

        // RET cc - Conditional return
        switch (y)
        {
        case 0:
            cc = !(*cpu.flags & CONST_ZERO_FLAG);
            break;
        case 1:
            cc = (*cpu.flags & CONST_ZERO_FLAG);
            break;
        case 2:
            cc = !(*cpu.flags & CONST_CARRY_FLAG);
            break;
        case 3:
            cc = (*cpu.flags & CONST_CARRY_FLAG);
            break;
        };
    }

    if (opcode == 0xcd || cc)
    {
        // Call nn
        cpu.call(arg1, arg2);
    }
}

void PUSH(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t p = (y & 6) >> 1;

    uint16_t value = *cpu.registers16_2[p];
    cpu.push(value);
}

void POP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.compute_current_opcode_groupings(x, y, z);
    uint8_t p = (y & 6) >> 1;

    *cpu.registers16_2[p] = cpu.pop();

    *cpu.flags &= 0xF0; // reset lower nibble after pop AF
    assert((int)(*cpu.flags & 0x0F) == 0);
}

void RET(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    cpu.compute_current_opcode_groupings(x, y, z);

    bool ret = false;
    if (z == 0)
    {
        // RET cc - Conditional return
        switch (y)
        {
        case 0:
            ret = !(*cpu.flags & CONST_ZERO_FLAG);
            break;
        case 1:
            ret = (*cpu.flags & CONST_ZERO_FLAG);
            break;
        case 2:
            ret = !(*cpu.flags & CONST_CARRY_FLAG);
            break;
        case 3:
            ret = (*cpu.flags & CONST_CARRY_FLAG);
            break;
        };
    }
    if (z == 1 || (z == 0 && ret))
    {
        // RET
        uint16_t address = cpu.pop();
        // Address stored in the stack is already the correct one, main loop will increment PC by 1 byte, so minus one
        // cpu.PC = address - 1;
        cpu.PC = address - cpu.next_instruction_relative_pos;
        // cpu.PC = address;
    }
    else if (z == 0 && !(ret))
    {
        return;
    }
    else
    {
        std::cout << "UNK" << std::endl;
        cpu.panic();
    }
}

void RETI(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint16_t address = cpu.pop();
    // Address stored in the stack is already the correct one, main loop will increment PC by 1 byte, so minus one
    cpu.PC = address - cpu.next_instruction_relative_pos;
    cpu.enable_interrupts();
}

void RST(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // Restart
    uint8_t opcode = cpu.current_opcode;
    cpu.compute_current_opcode_groupings(x, y, z);

    cpu.push(cpu.PC + 1); // Push next address to avoid loop
    cpu.PC = y * 8 - 1;

    // if (y * 8 == 0x38)
    // {
    //     std::cout << std::endl
    //               << "Reached RST 38H, there must be a mistake" << std::endl;
    //     cpu.panic();
    // }
}

// CB-prefixed

void RES(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    uint8_t *p_reg = cpu.registers[z];

    // Reset bit y in register z
    *p_reg &= ~(1 << y);
}

void BIT(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    if (!(*cpu.registers[z] & (1 << y)))
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags |= CONST_HALFCARRY_FLAG;

    // std::cout << std::bitset<8>(*cpu.flags) << std::endl;
}

void rotate_left(CPU &cpu, uint8_t *p_reg)
{
    // Set carry flag to the leftmost bit value
    *cpu.flags &= ~CONST_CARRY_FLAG;
    if (*p_reg & (1 << 7))
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    *p_reg <<= 1;

    if (*p_reg == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
}

void rotate_right(CPU &cpu, uint8_t *p_reg)
{

    uint8_t carry = (*cpu.flags & CONST_CARRY_FLAG) >> 4;
    // Set carry flag to the rightmost bit value
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;

    if (*p_reg & 1)
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    *p_reg >>= 1;
    *p_reg |= (carry << 7);

    // std::cout << std::endl << std::hex << (int)(*p_reg) << std::endl;

    if (*p_reg == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }
}

void RLA(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t carry = (*cpu.flags & CONST_CARRY_FLAG) >> 4;
    uint8_t *p_reg = cpu.registers[7]; // A
    // Set carry flag to the leftmost bit value
    *cpu.flags &= ~CONST_CARRY_FLAG;
    if (*p_reg & (1 << 7))
    {
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    *p_reg <<= 1;
    *p_reg += carry;

    if (*p_reg == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
}

void RLCA(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t value = *cpu.p_A;
    *cpu.flags |= (value >> 7) << 4;
    *cpu.p_A = value << 1 | value >> 7;

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_ZERO_FLAG;
    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void RRA(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    rotate_right(cpu, cpu.p_A); // register 'A'

    // According to BLARGG, the manual is wrong about flags
    // Z flag always set to zero, except for extented instructions
    // RLCA, RLA, RRCA, RRA instructions: the flags are described wrong in the CPU manual: after the execution C contains the rotated bit while N, H and Z are always 0. The equivalent instructions in the extended CB opcode space however set Z if the result is 0 and that's correctly documented.

    (*cpu.flags) &= ~CONST_ZERO_FLAG;
}

void RL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    rotate_left(cpu, cpu.registers[z]);
}

void RR(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);
    rotate_right(cpu, cpu.registers[z]);
}

void SRL(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    *cpu.flags &= ~CONST_ZERO_FLAG;
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    // *cpu.flags &= ~CONST_CARRY_FLAG;

    *cpu.flags = ((*cpu.get_register(z) & 1) << 4); // Bit 0 to carry
    *cpu.get_register(z) >>= 1;

    if (*cpu.registers[z] == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void SLA(CPU &cpu, uint8_t arg1, uint8_t arg2){
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    *cpu.flags = (((*cpu.get_register(z) >> 7) & 1) << 4); // Bit 7 to carry

    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;

    *cpu.get_register(z) <<= 1;

    *cpu.flags &= ~CONST_ZERO_FLAG;
    if (*cpu.registers[z] == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void DAA(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    // https: // ehaskins.com/2018-01-30%20Z80%20DAA/
    uint8_t correction = 0;

    // std::cout << std::endl;
    // std::cout << std::hex << (int) (*cpu.flags & CONST_HALFCARRY_FLAG) << std::endl;
    // std::cout << std::hex << (int) ((*cpu.p_A & 0x0f) > 0x09) << std::endl;
    // std::cout << std::hex << (int) (*cpu.p_A > 0x99) << std::endl;

    if ((*cpu.flags & CONST_HALFCARRY_FLAG) ||
        (!(*cpu.flags & CONST_SUBSTRACT_FLAG) && ((*cpu.p_A & 0x0f) > 0x09)))
    {

        correction |= 0x06;
    }
    if ((*cpu.flags & CONST_CARRY_FLAG) ||
        (!(*cpu.flags & CONST_SUBSTRACT_FLAG) && (*cpu.p_A > 0x99)))
    {
        correction |= 0x60;
        *cpu.flags |= CONST_CARRY_FLAG;
    }

    *cpu.p_A += (*cpu.flags & CONST_SUBSTRACT_FLAG) ? -correction : correction;

    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_ZERO_FLAG;
    if (*cpu.p_A == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
}

void SWAP(CPU &cpu, uint8_t arg1, uint8_t arg2)
{

    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    uint8_t *p_value;
    if (opcode == 0x36)
    {
        p_value = cpu.m.get_pointer(cpu.HL);
    }
    else
    {
        p_value = cpu.registers[z];
    }

    *p_value = ((*p_value & 0xF0) >> 4) | ((*p_value & 0x0F) << 4);

    if (*p_value == 0)
    {
        *cpu.flags |= CONST_ZERO_FLAG;
    }
    else
    {
        *cpu.flags &= ~CONST_ZERO_FLAG;
    }
    *cpu.flags &= ~CONST_SUBSTRACT_FLAG;
    *cpu.flags &= ~CONST_HALFCARRY_FLAG;
    *cpu.flags &= ~CONST_CARRY_FLAG;
}

void SET(CPU &cpu, uint8_t arg1, uint8_t arg2)
{
    uint8_t opcode = cpu.m.read(cpu.PC + 1);
    cpu.compute_opcode_groupings(opcode, x, y, z);

    // SET y, r[z]
    *cpu.get_register(z) |= (1 << y);
}
