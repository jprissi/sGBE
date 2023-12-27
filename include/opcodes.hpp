
    #ifndef OPCODES_H
    #define OPCODES_H

    #include <stdint.h>
    #include <string>
    #include "cpu.hpp"

    uint8_t decode(unsigned char opcode);

    void UNK(CPU &cpu, uint8_t arg1, uint8_t arg2);
    void RRA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void ADC(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RLCA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void DI(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RRCA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void CCF(CPU &cpu, uint8_t arg1, uint8_t arg2);
void CALL(CPU &cpu, uint8_t arg1, uint8_t arg2);
void CP(CPU &cpu, uint8_t arg1, uint8_t arg2);
void PUSH(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RST(CPU &cpu, uint8_t arg1, uint8_t arg2);
void LDH(CPU &cpu, uint8_t arg1, uint8_t arg2);
void EI(CPU &cpu, uint8_t arg1, uint8_t arg2);
void OR(CPU &cpu, uint8_t arg1, uint8_t arg2);
void XOR(CPU &cpu, uint8_t arg1, uint8_t arg2);
void HALT(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SBC(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SCF(CPU &cpu, uint8_t arg1, uint8_t arg2);
void CPL(CPU &cpu, uint8_t arg1, uint8_t arg2);
void JR(CPU &cpu, uint8_t arg1, uint8_t arg2);
void AND(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RETI(CPU &cpu, uint8_t arg1, uint8_t arg2);
void JP(CPU &cpu, uint8_t arg1, uint8_t arg2);
void INC(CPU &cpu, uint8_t arg1, uint8_t arg2);
void LD(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RLA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void ADD(CPU &cpu, uint8_t arg1, uint8_t arg2);
void NOP(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SUB(CPU &cpu, uint8_t arg1, uint8_t arg2);
void DAA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void DEC(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RET(CPU &cpu, uint8_t arg1, uint8_t arg2);
void POP(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RES(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SLA(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SWAP(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SRL(CPU &cpu, uint8_t arg1, uint8_t arg2);
void SET(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RL(CPU &cpu, uint8_t arg1, uint8_t arg2);
void BIT(CPU &cpu, uint8_t arg1, uint8_t arg2);
void RR(CPU &cpu, uint8_t arg1, uint8_t arg2);

    struct opcodes_s { // The opcode value is implicit in the array index
    uint8_t opcode;
    const char *mn; // Instruction mnemonic, not including data
    uint8_t length;
    uint8_t cycles;
    void (*func)(CPU &cpu, uint8_t arg1, uint8_t arg2); // Implement minimal set of mnemonics
    //char* label;
    std::string label;
    };

    // Array, hex opcode is array address
    const opcodes_s opcodes[] = {
    	{0x00, "NOP", 1, 4, &NOP, "NOP "},
	{0x01, "LD", 3, 12, &LD, "LD BC, d16"},
	{0x02, "LD", 1, 8, &LD, "LD (BC), A"},
	{0x03, "INC", 1, 8, &INC, "INC BC"},
	{0x04, "INC", 1, 4, &INC, "INC B"},
	{0x05, "DEC", 1, 4, &DEC, "DEC B"},
	{0x06, "LD", 2, 8, &LD, "LD B, d8"},
	{0x07, "RLCA", 1, 4, &RLCA, "RLCA "},
	{0x08, "LD", 3, 20, &LD, "LD (a16), SP"},
	{0x09, "ADD", 1, 8, &ADD, "ADD HL, BC"},
	{0x0a, "LD", 1, 8, &LD, "LD A, (BC)"},
	{0x0b, "DEC", 1, 8, &DEC, "DEC BC"},
	{0x0c, "INC", 1, 4, &INC, "INC C"},
	{0x0d, "DEC", 1, 4, &DEC, "DEC C"},
	{0x0e, "LD", 2, 8, &LD, "LD C, d8"},
	{0x0f, "RRCA", 1, 4, &RRCA, "RRCA "},
	{0x10, "STOP", 1, 4, &UNK, "STOP 0"},
	{0x11, "LD", 3, 12, &LD, "LD DE, d16"},
	{0x12, "LD", 1, 8, &LD, "LD (DE), A"},
	{0x13, "INC", 1, 8, &INC, "INC DE"},
	{0x14, "INC", 1, 4, &INC, "INC D"},
	{0x15, "DEC", 1, 4, &DEC, "DEC D"},
	{0x16, "LD", 2, 8, &LD, "LD D, d8"},
	{0x17, "RLA", 1, 4, &RLA, "RLA "},
	{0x18, "JR", 2, 12, &JR, "JR r8"},
	{0x19, "ADD", 1, 8, &ADD, "ADD HL, DE"},
	{0x1a, "LD", 1, 8, &LD, "LD A, (DE)"},
	{0x1b, "DEC", 1, 8, &DEC, "DEC DE"},
	{0x1c, "INC", 1, 4, &INC, "INC E"},
	{0x1d, "DEC", 1, 4, &DEC, "DEC E"},
	{0x1e, "LD", 2, 8, &LD, "LD E, d8"},
	{0x1f, "RRA", 1, 4, &RRA, "RRA "},
	{0x20, "JR", 2, 12, &JR, "JR NZ, r8"},
	{0x21, "LD", 3, 12, &LD, "LD HL, d16"},
	{0x22, "LD", 1, 8, &LD, "LD (HL+), A"},
	{0x23, "INC", 1, 8, &INC, "INC HL"},
	{0x24, "INC", 1, 4, &INC, "INC H"},
	{0x25, "DEC", 1, 4, &DEC, "DEC H"},
	{0x26, "LD", 2, 8, &LD, "LD H, d8"},
	{0x27, "DAA", 1, 4, &DAA, "DAA "},
	{0x28, "JR", 2, 12, &JR, "JR Z, r8"},
	{0x29, "ADD", 1, 8, &ADD, "ADD HL, HL"},
	{0x2a, "LD", 1, 8, &LD, "LD A, (HL+)"},
	{0x2b, "DEC", 1, 8, &DEC, "DEC HL"},
	{0x2c, "INC", 1, 4, &INC, "INC L"},
	{0x2d, "DEC", 1, 4, &DEC, "DEC L"},
	{0x2e, "LD", 2, 8, &LD, "LD L, d8"},
	{0x2f, "CPL", 1, 4, &CPL, "CPL "},
	{0x30, "JR", 2, 12, &JR, "JR NC, r8"},
	{0x31, "LD", 3, 12, &LD, "LD SP, d16"},
	{0x32, "LD", 1, 8, &LD, "LD (HL-), A"},
	{0x33, "INC", 1, 8, &INC, "INC SP"},
	{0x34, "INC", 1, 12, &INC, "INC (HL)"},
	{0x35, "DEC", 1, 12, &DEC, "DEC (HL)"},
	{0x36, "LD", 2, 12, &LD, "LD (HL), d8"},
	{0x37, "SCF", 1, 4, &SCF, "SCF "},
	{0x38, "JR", 2, 12, &JR, "JR C, r8"},
	{0x39, "ADD", 1, 8, &ADD, "ADD HL, SP"},
	{0x3a, "LD", 1, 8, &LD, "LD A, (HL-)"},
	{0x3b, "DEC", 1, 8, &DEC, "DEC SP"},
	{0x3c, "INC", 1, 4, &INC, "INC A"},
	{0x3d, "DEC", 1, 4, &DEC, "DEC A"},
	{0x3e, "LD", 2, 8, &LD, "LD A, d8"},
	{0x3f, "CCF", 1, 4, &CCF, "CCF "},
	{0x40, "LD", 1, 4, &LD, "LD B, B"},
	{0x41, "LD", 1, 4, &LD, "LD B, C"},
	{0x42, "LD", 1, 4, &LD, "LD B, D"},
	{0x43, "LD", 1, 4, &LD, "LD B, E"},
	{0x44, "LD", 1, 4, &LD, "LD B, H"},
	{0x45, "LD", 1, 4, &LD, "LD B, L"},
	{0x46, "LD", 1, 8, &LD, "LD B, (HL)"},
	{0x47, "LD", 1, 4, &LD, "LD B, A"},
	{0x48, "LD", 1, 4, &LD, "LD C, B"},
	{0x49, "LD", 1, 4, &LD, "LD C, C"},
	{0x4a, "LD", 1, 4, &LD, "LD C, D"},
	{0x4b, "LD", 1, 4, &LD, "LD C, E"},
	{0x4c, "LD", 1, 4, &LD, "LD C, H"},
	{0x4d, "LD", 1, 4, &LD, "LD C, L"},
	{0x4e, "LD", 1, 8, &LD, "LD C, (HL)"},
	{0x4f, "LD", 1, 4, &LD, "LD C, A"},
	{0x50, "LD", 1, 4, &LD, "LD D, B"},
	{0x51, "LD", 1, 4, &LD, "LD D, C"},
	{0x52, "LD", 1, 4, &LD, "LD D, D"},
	{0x53, "LD", 1, 4, &LD, "LD D, E"},
	{0x54, "LD", 1, 4, &LD, "LD D, H"},
	{0x55, "LD", 1, 4, &LD, "LD D, L"},
	{0x56, "LD", 1, 8, &LD, "LD D, (HL)"},
	{0x57, "LD", 1, 4, &LD, "LD D, A"},
	{0x58, "LD", 1, 4, &LD, "LD E, B"},
	{0x59, "LD", 1, 4, &LD, "LD E, C"},
	{0x5a, "LD", 1, 4, &LD, "LD E, D"},
	{0x5b, "LD", 1, 4, &LD, "LD E, E"},
	{0x5c, "LD", 1, 4, &LD, "LD E, H"},
	{0x5d, "LD", 1, 4, &LD, "LD E, L"},
	{0x5e, "LD", 1, 8, &LD, "LD E, (HL)"},
	{0x5f, "LD", 1, 4, &LD, "LD E, A"},
	{0x60, "LD", 1, 4, &LD, "LD H, B"},
	{0x61, "LD", 1, 4, &LD, "LD H, C"},
	{0x62, "LD", 1, 4, &LD, "LD H, D"},
	{0x63, "LD", 1, 4, &LD, "LD H, E"},
	{0x64, "LD", 1, 4, &LD, "LD H, H"},
	{0x65, "LD", 1, 4, &LD, "LD H, L"},
	{0x66, "LD", 1, 8, &LD, "LD H, (HL)"},
	{0x67, "LD", 1, 4, &LD, "LD H, A"},
	{0x68, "LD", 1, 4, &LD, "LD L, B"},
	{0x69, "LD", 1, 4, &LD, "LD L, C"},
	{0x6a, "LD", 1, 4, &LD, "LD L, D"},
	{0x6b, "LD", 1, 4, &LD, "LD L, E"},
	{0x6c, "LD", 1, 4, &LD, "LD L, H"},
	{0x6d, "LD", 1, 4, &LD, "LD L, L"},
	{0x6e, "LD", 1, 8, &LD, "LD L, (HL)"},
	{0x6f, "LD", 1, 4, &LD, "LD L, A"},
	{0x70, "LD", 1, 8, &LD, "LD (HL), B"},
	{0x71, "LD", 1, 8, &LD, "LD (HL), C"},
	{0x72, "LD", 1, 8, &LD, "LD (HL), D"},
	{0x73, "LD", 1, 8, &LD, "LD (HL), E"},
	{0x74, "LD", 1, 8, &LD, "LD (HL), H"},
	{0x75, "LD", 1, 8, &LD, "LD (HL), L"},
	{0x76, "HALT", 1, 4, &HALT, "HALT "},
	{0x77, "LD", 1, 8, &LD, "LD (HL), A"},
	{0x78, "LD", 1, 4, &LD, "LD A, B"},
	{0x79, "LD", 1, 4, &LD, "LD A, C"},
	{0x7a, "LD", 1, 4, &LD, "LD A, D"},
	{0x7b, "LD", 1, 4, &LD, "LD A, E"},
	{0x7c, "LD", 1, 4, &LD, "LD A, H"},
	{0x7d, "LD", 1, 4, &LD, "LD A, L"},
	{0x7e, "LD", 1, 8, &LD, "LD A, (HL)"},
	{0x7f, "LD", 1, 4, &LD, "LD A, A"},
	{0x80, "ADD", 1, 4, &ADD, "ADD A, B"},
	{0x81, "ADD", 1, 4, &ADD, "ADD A, C"},
	{0x82, "ADD", 1, 4, &ADD, "ADD A, D"},
	{0x83, "ADD", 1, 4, &ADD, "ADD A, E"},
	{0x84, "ADD", 1, 4, &ADD, "ADD A, H"},
	{0x85, "ADD", 1, 4, &ADD, "ADD A, L"},
	{0x86, "ADD", 1, 8, &ADD, "ADD A, (HL)"},
	{0x87, "ADD", 1, 4, &ADD, "ADD A, A"},
	{0x88, "ADC", 1, 4, &ADC, "ADC A, B"},
	{0x89, "ADC", 1, 4, &ADC, "ADC A, C"},
	{0x8a, "ADC", 1, 4, &ADC, "ADC A, D"},
	{0x8b, "ADC", 1, 4, &ADC, "ADC A, E"},
	{0x8c, "ADC", 1, 4, &ADC, "ADC A, H"},
	{0x8d, "ADC", 1, 4, &ADC, "ADC A, L"},
	{0x8e, "ADC", 1, 8, &ADC, "ADC A, (HL)"},
	{0x8f, "ADC", 1, 4, &ADC, "ADC A, A"},
	{0x90, "SUB", 1, 4, &SUB, "SUB B"},
	{0x91, "SUB", 1, 4, &SUB, "SUB C"},
	{0x92, "SUB", 1, 4, &SUB, "SUB D"},
	{0x93, "SUB", 1, 4, &SUB, "SUB E"},
	{0x94, "SUB", 1, 4, &SUB, "SUB H"},
	{0x95, "SUB", 1, 4, &SUB, "SUB L"},
	{0x96, "SUB", 1, 8, &SUB, "SUB (HL)"},
	{0x97, "SUB", 1, 4, &SUB, "SUB A"},
	{0x98, "SBC", 1, 4, &SBC, "SBC A, B"},
	{0x99, "SBC", 1, 4, &SBC, "SBC A, C"},
	{0x9a, "SBC", 1, 4, &SBC, "SBC A, D"},
	{0x9b, "SBC", 1, 4, &SBC, "SBC A, E"},
	{0x9c, "SBC", 1, 4, &SBC, "SBC A, H"},
	{0x9d, "SBC", 1, 4, &SBC, "SBC A, L"},
	{0x9e, "SBC", 1, 8, &SBC, "SBC A, (HL)"},
	{0x9f, "SBC", 1, 4, &SBC, "SBC A, A"},
	{0xa0, "AND", 1, 4, &AND, "AND B"},
	{0xa1, "AND", 1, 4, &AND, "AND C"},
	{0xa2, "AND", 1, 4, &AND, "AND D"},
	{0xa3, "AND", 1, 4, &AND, "AND E"},
	{0xa4, "AND", 1, 4, &AND, "AND H"},
	{0xa5, "AND", 1, 4, &AND, "AND L"},
	{0xa6, "AND", 1, 8, &AND, "AND (HL)"},
	{0xa7, "AND", 1, 4, &AND, "AND A"},
	{0xa8, "XOR", 1, 4, &XOR, "XOR B"},
	{0xa9, "XOR", 1, 4, &XOR, "XOR C"},
	{0xaa, "XOR", 1, 4, &XOR, "XOR D"},
	{0xab, "XOR", 1, 4, &XOR, "XOR E"},
	{0xac, "XOR", 1, 4, &XOR, "XOR H"},
	{0xad, "XOR", 1, 4, &XOR, "XOR L"},
	{0xae, "XOR", 1, 8, &XOR, "XOR (HL)"},
	{0xaf, "XOR", 1, 4, &XOR, "XOR A"},
	{0xb0, "OR", 1, 4, &OR, "OR B"},
	{0xb1, "OR", 1, 4, &OR, "OR C"},
	{0xb2, "OR", 1, 4, &OR, "OR D"},
	{0xb3, "OR", 1, 4, &OR, "OR E"},
	{0xb4, "OR", 1, 4, &OR, "OR H"},
	{0xb5, "OR", 1, 4, &OR, "OR L"},
	{0xb6, "OR", 1, 8, &OR, "OR (HL)"},
	{0xb7, "OR", 1, 4, &OR, "OR A"},
	{0xb8, "CP", 1, 4, &CP, "CP B"},
	{0xb9, "CP", 1, 4, &CP, "CP C"},
	{0xba, "CP", 1, 4, &CP, "CP D"},
	{0xbb, "CP", 1, 4, &CP, "CP E"},
	{0xbc, "CP", 1, 4, &CP, "CP H"},
	{0xbd, "CP", 1, 4, &CP, "CP L"},
	{0xbe, "CP", 1, 8, &CP, "CP (HL)"},
	{0xbf, "CP", 1, 4, &CP, "CP A"},
	{0xc0, "RET", 1, 20, &RET, "RET NZ"},
	{0xc1, "POP", 1, 12, &POP, "POP BC"},
	{0xc2, "JP", 3, 16, &JP, "JP NZ, a16"},
	{0xc3, "JP", 3, 16, &JP, "JP a16"},
	{0xc4, "CALL", 3, 24, &CALL, "CALL NZ, a16"},
	{0xc5, "PUSH", 1, 16, &PUSH, "PUSH BC"},
	{0xc6, "ADD", 2, 8, &ADD, "ADD A, d8"},
	{0xc7, "RST", 1, 16, &RST, "RST 00H"},
	{0xc8, "RET", 1, 20, &RET, "RET Z"},
	{0xc9, "RET", 1, 16, &RET, "RET "},
	{0xca, "JP", 3, 16, &JP, "JP Z, a16"},
	{0xcb, "PREFIX", 1, 4, &UNK, "PREFIX CB"},
	{0xcc, "CALL", 3, 24, &CALL, "CALL Z, a16"},
	{0xcd, "CALL", 3, 24, &CALL, "CALL a16"},
	{0xce, "ADC", 2, 8, &ADC, "ADC A, d8"},
	{0xcf, "RST", 1, 16, &RST, "RST 08H"},
	{0xd0, "RET", 1, 20, &RET, "RET NC"},
	{0xd1, "POP", 1, 12, &POP, "POP DE"},
	{0xd2, "JP", 3, 16, &JP, "JP NC, a16"},
	{0xd3, "UNK", 0, 0, &UNK, "UNK"},
	{0xd4, "CALL", 3, 24, &CALL, "CALL NC, a16"},
	{0xd5, "PUSH", 1, 16, &PUSH, "PUSH DE"},
	{0xd6, "SUB", 2, 8, &SUB, "SUB d8"},
	{0xd7, "RST", 1, 16, &RST, "RST 10H"},
	{0xd8, "RET", 1, 20, &RET, "RET C"},
	{0xd9, "RETI", 1, 16, &RETI, "RETI "},
	{0xda, "JP", 3, 16, &JP, "JP C, a16"},
	{0xdb, "UNK", 0, 0, &UNK, "UNK"},
	{0xdc, "CALL", 3, 24, &CALL, "CALL C, a16"},
	{0xdd, "UNK", 0, 0, &UNK, "UNK"},
	{0xde, "SBC", 2, 8, &SBC, "SBC A, d8"},
	{0xdf, "RST", 1, 16, &RST, "RST 18H"},
	{0xe0, "LDH", 2, 12, &LDH, "LDH (a8), A"},
	{0xe1, "POP", 1, 12, &POP, "POP HL"},
	{0xe2, "LD", 1, 8, &LD, "LD (C), A"},
	{0xe3, "UNK", 0, 0, &UNK, "UNK"},
	{0xe4, "UNK", 0, 0, &UNK, "UNK"},
	{0xe5, "PUSH", 1, 16, &PUSH, "PUSH HL"},
	{0xe6, "AND", 2, 8, &AND, "AND d8"},
	{0xe7, "RST", 1, 16, &RST, "RST 20H"},
	{0xe8, "ADD", 2, 16, &ADD, "ADD SP, r8"},
	{0xe9, "JP", 1, 4, &JP, "JP (HL)"},
	{0xea, "LD", 3, 16, &LD, "LD (a16), A"},
	{0xeb, "UNK", 0, 0, &UNK, "UNK"},
	{0xec, "UNK", 0, 0, &UNK, "UNK"},
	{0xed, "UNK", 0, 0, &UNK, "UNK"},
	{0xee, "XOR", 2, 8, &XOR, "XOR d8"},
	{0xef, "RST", 1, 16, &RST, "RST 28H"},
	{0xf0, "LDH", 2, 12, &LDH, "LDH A, (a8)"},
	{0xf1, "POP", 1, 12, &POP, "POP AF"},
	{0xf2, "LD", 1, 8, &LD, "LD A, (C)"},
	{0xf3, "DI", 1, 4, &DI, "DI "},
	{0xf4, "UNK", 0, 0, &UNK, "UNK"},
	{0xf5, "PUSH", 1, 16, &PUSH, "PUSH AF"},
	{0xf6, "OR", 2, 8, &OR, "OR d8"},
	{0xf7, "RST", 1, 16, &RST, "RST 30H"},
	{0xf8, "LD", 2, 12, &LD, "LD HL, SP+r8"},
	{0xf9, "LD", 1, 8, &LD, "LD SP, HL"},
	{0xfa, "LD", 3, 16, &LD, "LD A, (a16)"},
	{0xfb, "EI", 1, 4, &EI, "EI "},
	{0xfc, "UNK", 0, 0, &UNK, "UNK"},
	{0xfd, "UNK", 0, 0, &UNK, "UNK"},
	{0xfe, "CP", 2, 8, &CP, "CP d8"},
	{0xff, "RST", 1, 16, &RST, "RST 38H"},

    };

    const opcodes_s prefixed_opcodes[] = {
    	{0x00, "RLC", 2, 8, &UNK, "RLC B"},
	{0x01, "RLC", 2, 8, &UNK, "RLC C"},
	{0x02, "RLC", 2, 8, &UNK, "RLC D"},
	{0x03, "RLC", 2, 8, &UNK, "RLC E"},
	{0x04, "RLC", 2, 8, &UNK, "RLC H"},
	{0x05, "RLC", 2, 8, &UNK, "RLC L"},
	{0x06, "RLC", 2, 16, &UNK, "RLC (HL)"},
	{0x07, "RLC", 2, 8, &UNK, "RLC A"},
	{0x08, "RRC", 2, 8, &UNK, "RRC B"},
	{0x09, "RRC", 2, 8, &UNK, "RRC C"},
	{0x0a, "RRC", 2, 8, &UNK, "RRC D"},
	{0x0b, "RRC", 2, 8, &UNK, "RRC E"},
	{0x0c, "RRC", 2, 8, &UNK, "RRC H"},
	{0x0d, "RRC", 2, 8, &UNK, "RRC L"},
	{0x0e, "RRC", 2, 16, &UNK, "RRC (HL)"},
	{0x0f, "RRC", 2, 8, &UNK, "RRC A"},
	{0x10, "RL", 2, 8, &RL, "RL B"},
	{0x11, "RL", 2, 8, &RL, "RL C"},
	{0x12, "RL", 2, 8, &RL, "RL D"},
	{0x13, "RL", 2, 8, &RL, "RL E"},
	{0x14, "RL", 2, 8, &RL, "RL H"},
	{0x15, "RL", 2, 8, &RL, "RL L"},
	{0x16, "RL", 2, 16, &RL, "RL (HL)"},
	{0x17, "RL", 2, 8, &RL, "RL A"},
	{0x18, "RR", 2, 8, &RR, "RR B"},
	{0x19, "RR", 2, 8, &RR, "RR C"},
	{0x1a, "RR", 2, 8, &RR, "RR D"},
	{0x1b, "RR", 2, 8, &RR, "RR E"},
	{0x1c, "RR", 2, 8, &RR, "RR H"},
	{0x1d, "RR", 2, 8, &RR, "RR L"},
	{0x1e, "RR", 2, 16, &RR, "RR (HL)"},
	{0x1f, "RR", 2, 8, &RR, "RR A"},
	{0x20, "SLA", 2, 8, &SLA, "SLA B"},
	{0x21, "SLA", 2, 8, &SLA, "SLA C"},
	{0x22, "SLA", 2, 8, &SLA, "SLA D"},
	{0x23, "SLA", 2, 8, &SLA, "SLA E"},
	{0x24, "SLA", 2, 8, &SLA, "SLA H"},
	{0x25, "SLA", 2, 8, &SLA, "SLA L"},
	{0x26, "SLA", 2, 16, &SLA, "SLA (HL)"},
	{0x27, "SLA", 2, 8, &SLA, "SLA A"},
	{0x28, "SRA", 2, 8, &UNK, "SRA B"},
	{0x29, "SRA", 2, 8, &UNK, "SRA C"},
	{0x2a, "SRA", 2, 8, &UNK, "SRA D"},
	{0x2b, "SRA", 2, 8, &UNK, "SRA E"},
	{0x2c, "SRA", 2, 8, &UNK, "SRA H"},
	{0x2d, "SRA", 2, 8, &UNK, "SRA L"},
	{0x2e, "SRA", 2, 16, &UNK, "SRA (HL)"},
	{0x2f, "SRA", 2, 8, &UNK, "SRA A"},
	{0x30, "SWAP", 2, 8, &SWAP, "SWAP B"},
	{0x31, "SWAP", 2, 8, &SWAP, "SWAP C"},
	{0x32, "SWAP", 2, 8, &SWAP, "SWAP D"},
	{0x33, "SWAP", 2, 8, &SWAP, "SWAP E"},
	{0x34, "SWAP", 2, 8, &SWAP, "SWAP H"},
	{0x35, "SWAP", 2, 8, &SWAP, "SWAP L"},
	{0x36, "SWAP", 2, 16, &SWAP, "SWAP (HL)"},
	{0x37, "SWAP", 2, 8, &SWAP, "SWAP A"},
	{0x38, "SRL", 2, 8, &SRL, "SRL B"},
	{0x39, "SRL", 2, 8, &SRL, "SRL C"},
	{0x3a, "SRL", 2, 8, &SRL, "SRL D"},
	{0x3b, "SRL", 2, 8, &SRL, "SRL E"},
	{0x3c, "SRL", 2, 8, &SRL, "SRL H"},
	{0x3d, "SRL", 2, 8, &SRL, "SRL L"},
	{0x3e, "SRL", 2, 16, &SRL, "SRL (HL)"},
	{0x3f, "SRL", 2, 8, &SRL, "SRL A"},
	{0x40, "BIT", 2, 8, &BIT, "BIT 0, B"},
	{0x41, "BIT", 2, 8, &BIT, "BIT 0, C"},
	{0x42, "BIT", 2, 8, &BIT, "BIT 0, D"},
	{0x43, "BIT", 2, 8, &BIT, "BIT 0, E"},
	{0x44, "BIT", 2, 8, &BIT, "BIT 0, H"},
	{0x45, "BIT", 2, 8, &BIT, "BIT 0, L"},
	{0x46, "BIT", 2, 16, &BIT, "BIT 0, (HL)"},
	{0x47, "BIT", 2, 8, &BIT, "BIT 0, A"},
	{0x48, "BIT", 2, 8, &BIT, "BIT 1, B"},
	{0x49, "BIT", 2, 8, &BIT, "BIT 1, C"},
	{0x4a, "BIT", 2, 8, &BIT, "BIT 1, D"},
	{0x4b, "BIT", 2, 8, &BIT, "BIT 1, E"},
	{0x4c, "BIT", 2, 8, &BIT, "BIT 1, H"},
	{0x4d, "BIT", 2, 8, &BIT, "BIT 1, L"},
	{0x4e, "BIT", 2, 16, &BIT, "BIT 1, (HL)"},
	{0x4f, "BIT", 2, 8, &BIT, "BIT 1, A"},
	{0x50, "BIT", 2, 8, &BIT, "BIT 2, B"},
	{0x51, "BIT", 2, 8, &BIT, "BIT 2, C"},
	{0x52, "BIT", 2, 8, &BIT, "BIT 2, D"},
	{0x53, "BIT", 2, 8, &BIT, "BIT 2, E"},
	{0x54, "BIT", 2, 8, &BIT, "BIT 2, H"},
	{0x55, "BIT", 2, 8, &BIT, "BIT 2, L"},
	{0x56, "BIT", 2, 16, &BIT, "BIT 2, (HL)"},
	{0x57, "BIT", 2, 8, &BIT, "BIT 2, A"},
	{0x58, "BIT", 2, 8, &BIT, "BIT 3, B"},
	{0x59, "BIT", 2, 8, &BIT, "BIT 3, C"},
	{0x5a, "BIT", 2, 8, &BIT, "BIT 3, D"},
	{0x5b, "BIT", 2, 8, &BIT, "BIT 3, E"},
	{0x5c, "BIT", 2, 8, &BIT, "BIT 3, H"},
	{0x5d, "BIT", 2, 8, &BIT, "BIT 3, L"},
	{0x5e, "BIT", 2, 16, &BIT, "BIT 3, (HL)"},
	{0x5f, "BIT", 2, 8, &BIT, "BIT 3, A"},
	{0x60, "BIT", 2, 8, &BIT, "BIT 4, B"},
	{0x61, "BIT", 2, 8, &BIT, "BIT 4, C"},
	{0x62, "BIT", 2, 8, &BIT, "BIT 4, D"},
	{0x63, "BIT", 2, 8, &BIT, "BIT 4, E"},
	{0x64, "BIT", 2, 8, &BIT, "BIT 4, H"},
	{0x65, "BIT", 2, 8, &BIT, "BIT 4, L"},
	{0x66, "BIT", 2, 16, &BIT, "BIT 4, (HL)"},
	{0x67, "BIT", 2, 8, &BIT, "BIT 4, A"},
	{0x68, "BIT", 2, 8, &BIT, "BIT 5, B"},
	{0x69, "BIT", 2, 8, &BIT, "BIT 5, C"},
	{0x6a, "BIT", 2, 8, &BIT, "BIT 5, D"},
	{0x6b, "BIT", 2, 8, &BIT, "BIT 5, E"},
	{0x6c, "BIT", 2, 8, &BIT, "BIT 5, H"},
	{0x6d, "BIT", 2, 8, &BIT, "BIT 5, L"},
	{0x6e, "BIT", 2, 16, &BIT, "BIT 5, (HL)"},
	{0x6f, "BIT", 2, 8, &BIT, "BIT 5, A"},
	{0x70, "BIT", 2, 8, &BIT, "BIT 6, B"},
	{0x71, "BIT", 2, 8, &BIT, "BIT 6, C"},
	{0x72, "BIT", 2, 8, &BIT, "BIT 6, D"},
	{0x73, "BIT", 2, 8, &BIT, "BIT 6, E"},
	{0x74, "BIT", 2, 8, &BIT, "BIT 6, H"},
	{0x75, "BIT", 2, 8, &BIT, "BIT 6, L"},
	{0x76, "BIT", 2, 16, &BIT, "BIT 6, (HL)"},
	{0x77, "BIT", 2, 8, &BIT, "BIT 6, A"},
	{0x78, "BIT", 2, 8, &BIT, "BIT 7, B"},
	{0x79, "BIT", 2, 8, &BIT, "BIT 7, C"},
	{0x7a, "BIT", 2, 8, &BIT, "BIT 7, D"},
	{0x7b, "BIT", 2, 8, &BIT, "BIT 7, E"},
	{0x7c, "BIT", 2, 8, &BIT, "BIT 7, H"},
	{0x7d, "BIT", 2, 8, &BIT, "BIT 7, L"},
	{0x7e, "BIT", 2, 16, &BIT, "BIT 7, (HL)"},
	{0x7f, "BIT", 2, 8, &BIT, "BIT 7, A"},
	{0x80, "RES", 2, 8, &RES, "RES 0, B"},
	{0x81, "RES", 2, 8, &RES, "RES 0, C"},
	{0x82, "RES", 2, 8, &RES, "RES 0, D"},
	{0x83, "RES", 2, 8, &RES, "RES 0, E"},
	{0x84, "RES", 2, 8, &RES, "RES 0, H"},
	{0x85, "RES", 2, 8, &RES, "RES 0, L"},
	{0x86, "RES", 2, 16, &RES, "RES 0, (HL)"},
	{0x87, "RES", 2, 8, &RES, "RES 0, A"},
	{0x88, "RES", 2, 8, &RES, "RES 1, B"},
	{0x89, "RES", 2, 8, &RES, "RES 1, C"},
	{0x8a, "RES", 2, 8, &RES, "RES 1, D"},
	{0x8b, "RES", 2, 8, &RES, "RES 1, E"},
	{0x8c, "RES", 2, 8, &RES, "RES 1, H"},
	{0x8d, "RES", 2, 8, &RES, "RES 1, L"},
	{0x8e, "RES", 2, 16, &RES, "RES 1, (HL)"},
	{0x8f, "RES", 2, 8, &RES, "RES 1, A"},
	{0x90, "RES", 2, 8, &RES, "RES 2, B"},
	{0x91, "RES", 2, 8, &RES, "RES 2, C"},
	{0x92, "RES", 2, 8, &RES, "RES 2, D"},
	{0x93, "RES", 2, 8, &RES, "RES 2, E"},
	{0x94, "RES", 2, 8, &RES, "RES 2, H"},
	{0x95, "RES", 2, 8, &RES, "RES 2, L"},
	{0x96, "RES", 2, 16, &RES, "RES 2, (HL)"},
	{0x97, "RES", 2, 8, &RES, "RES 2, A"},
	{0x98, "RES", 2, 8, &RES, "RES 3, B"},
	{0x99, "RES", 2, 8, &RES, "RES 3, C"},
	{0x9a, "RES", 2, 8, &RES, "RES 3, D"},
	{0x9b, "RES", 2, 8, &RES, "RES 3, E"},
	{0x9c, "RES", 2, 8, &RES, "RES 3, H"},
	{0x9d, "RES", 2, 8, &RES, "RES 3, L"},
	{0x9e, "RES", 2, 16, &RES, "RES 3, (HL)"},
	{0x9f, "RES", 2, 8, &RES, "RES 3, A"},
	{0xa0, "RES", 2, 8, &RES, "RES 4, B"},
	{0xa1, "RES", 2, 8, &RES, "RES 4, C"},
	{0xa2, "RES", 2, 8, &RES, "RES 4, D"},
	{0xa3, "RES", 2, 8, &RES, "RES 4, E"},
	{0xa4, "RES", 2, 8, &RES, "RES 4, H"},
	{0xa5, "RES", 2, 8, &RES, "RES 4, L"},
	{0xa6, "RES", 2, 16, &RES, "RES 4, (HL)"},
	{0xa7, "RES", 2, 8, &RES, "RES 4, A"},
	{0xa8, "RES", 2, 8, &RES, "RES 5, B"},
	{0xa9, "RES", 2, 8, &RES, "RES 5, C"},
	{0xaa, "RES", 2, 8, &RES, "RES 5, D"},
	{0xab, "RES", 2, 8, &RES, "RES 5, E"},
	{0xac, "RES", 2, 8, &RES, "RES 5, H"},
	{0xad, "RES", 2, 8, &RES, "RES 5, L"},
	{0xae, "RES", 2, 16, &RES, "RES 5, (HL)"},
	{0xaf, "RES", 2, 8, &RES, "RES 5, A"},
	{0xb0, "RES", 2, 8, &RES, "RES 6, B"},
	{0xb1, "RES", 2, 8, &RES, "RES 6, C"},
	{0xb2, "RES", 2, 8, &RES, "RES 6, D"},
	{0xb3, "RES", 2, 8, &RES, "RES 6, E"},
	{0xb4, "RES", 2, 8, &RES, "RES 6, H"},
	{0xb5, "RES", 2, 8, &RES, "RES 6, L"},
	{0xb6, "RES", 2, 16, &RES, "RES 6, (HL)"},
	{0xb7, "RES", 2, 8, &RES, "RES 6, A"},
	{0xb8, "RES", 2, 8, &RES, "RES 7, B"},
	{0xb9, "RES", 2, 8, &RES, "RES 7, C"},
	{0xba, "RES", 2, 8, &RES, "RES 7, D"},
	{0xbb, "RES", 2, 8, &RES, "RES 7, E"},
	{0xbc, "RES", 2, 8, &RES, "RES 7, H"},
	{0xbd, "RES", 2, 8, &RES, "RES 7, L"},
	{0xbe, "RES", 2, 16, &RES, "RES 7, (HL)"},
	{0xbf, "RES", 2, 8, &RES, "RES 7, A"},
	{0xc0, "SET", 2, 8, &SET, "SET 0, B"},
	{0xc1, "SET", 2, 8, &SET, "SET 0, C"},
	{0xc2, "SET", 2, 8, &SET, "SET 0, D"},
	{0xc3, "SET", 2, 8, &SET, "SET 0, E"},
	{0xc4, "SET", 2, 8, &SET, "SET 0, H"},
	{0xc5, "SET", 2, 8, &SET, "SET 0, L"},
	{0xc6, "SET", 2, 16, &SET, "SET 0, (HL)"},
	{0xc7, "SET", 2, 8, &SET, "SET 0, A"},
	{0xc8, "SET", 2, 8, &SET, "SET 1, B"},
	{0xc9, "SET", 2, 8, &SET, "SET 1, C"},
	{0xca, "SET", 2, 8, &SET, "SET 1, D"},
	{0xcb, "SET", 2, 8, &SET, "SET 1, E"},
	{0xcc, "SET", 2, 8, &SET, "SET 1, H"},
	{0xcd, "SET", 2, 8, &SET, "SET 1, L"},
	{0xce, "SET", 2, 16, &SET, "SET 1, (HL)"},
	{0xcf, "SET", 2, 8, &SET, "SET 1, A"},
	{0xd0, "SET", 2, 8, &SET, "SET 2, B"},
	{0xd1, "SET", 2, 8, &SET, "SET 2, C"},
	{0xd2, "SET", 2, 8, &SET, "SET 2, D"},
	{0xd3, "SET", 2, 8, &SET, "SET 2, E"},
	{0xd4, "SET", 2, 8, &SET, "SET 2, H"},
	{0xd5, "SET", 2, 8, &SET, "SET 2, L"},
	{0xd6, "SET", 2, 16, &SET, "SET 2, (HL)"},
	{0xd7, "SET", 2, 8, &SET, "SET 2, A"},
	{0xd8, "SET", 2, 8, &SET, "SET 3, B"},
	{0xd9, "SET", 2, 8, &SET, "SET 3, C"},
	{0xda, "SET", 2, 8, &SET, "SET 3, D"},
	{0xdb, "SET", 2, 8, &SET, "SET 3, E"},
	{0xdc, "SET", 2, 8, &SET, "SET 3, H"},
	{0xdd, "SET", 2, 8, &SET, "SET 3, L"},
	{0xde, "SET", 2, 16, &SET, "SET 3, (HL)"},
	{0xdf, "SET", 2, 8, &SET, "SET 3, A"},
	{0xe0, "SET", 2, 8, &SET, "SET 4, B"},
	{0xe1, "SET", 2, 8, &SET, "SET 4, C"},
	{0xe2, "SET", 2, 8, &SET, "SET 4, D"},
	{0xe3, "SET", 2, 8, &SET, "SET 4, E"},
	{0xe4, "SET", 2, 8, &SET, "SET 4, H"},
	{0xe5, "SET", 2, 8, &SET, "SET 4, L"},
	{0xe6, "SET", 2, 16, &SET, "SET 4, (HL)"},
	{0xe7, "SET", 2, 8, &SET, "SET 4, A"},
	{0xe8, "SET", 2, 8, &SET, "SET 5, B"},
	{0xe9, "SET", 2, 8, &SET, "SET 5, C"},
	{0xea, "SET", 2, 8, &SET, "SET 5, D"},
	{0xeb, "SET", 2, 8, &SET, "SET 5, E"},
	{0xec, "SET", 2, 8, &SET, "SET 5, H"},
	{0xed, "SET", 2, 8, &SET, "SET 5, L"},
	{0xee, "SET", 2, 16, &SET, "SET 5, (HL)"},
	{0xef, "SET", 2, 8, &SET, "SET 5, A"},
	{0xf0, "SET", 2, 8, &SET, "SET 6, B"},
	{0xf1, "SET", 2, 8, &SET, "SET 6, C"},
	{0xf2, "SET", 2, 8, &SET, "SET 6, D"},
	{0xf3, "SET", 2, 8, &SET, "SET 6, E"},
	{0xf4, "SET", 2, 8, &SET, "SET 6, H"},
	{0xf5, "SET", 2, 8, &SET, "SET 6, L"},
	{0xf6, "SET", 2, 16, &SET, "SET 6, (HL)"},
	{0xf7, "SET", 2, 8, &SET, "SET 6, A"},
	{0xf8, "SET", 2, 8, &SET, "SET 7, B"},
	{0xf9, "SET", 2, 8, &SET, "SET 7, C"},
	{0xfa, "SET", 2, 8, &SET, "SET 7, D"},
	{0xfb, "SET", 2, 8, &SET, "SET 7, E"},
	{0xfc, "SET", 2, 8, &SET, "SET 7, H"},
	{0xfd, "SET", 2, 8, &SET, "SET 7, L"},
	{0xfe, "SET", 2, 16, &SET, "SET 7, (HL)"},
	{0xff, "SET", 2, 8, &SET, "SET 7, A"},

    };

    #endif
    