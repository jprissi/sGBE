#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include "cpu.hpp"

struct opcodes_s { // The opcode value is implicit in the array index
  uint8_t opcode;
  const char *mn; // Instruction mnemonic, not including data
  uint8_t length;
  uint8_t cycles;
  void (*func)(CPU &cpu, uint8_t arg1, uint8_t arg2); // Implement minimal set of mnemonics
};

  void SUB(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void DEC(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void CALL(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void CPL(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void SBC(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void LD(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void NOP(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void JP(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void INC(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void JR(CPU &cpu, uint8_t arg1, uint8_t arg2);

  void XOR(CPU &cpu, uint8_t arg1, uint8_t arg2);

  uint8_t decode(unsigned char opcode);

  void UNK(CPU &cpu, uint8_t arg1, uint8_t arg2);

// Array, hex opcode is array address
  const opcodes_s opcodes[] = {
	  {0x00, "NOP", 1, 4, &NOP},
	  {0x01, "LD", 3, 12, &LD},
	  {0x02, "LD", 1, 8, &LD},
	  {0x03, "INC", 1, 8, &INC},
	  {0x04, "INC", 1, 4, &INC},
	  {0x05, "DEC", 1, 4, &DEC},
	  {0x06, "LD", 2, 8, &LD},
	  {0x07, "RLCA", 1, 4, &UNK},
	  {0x08, "LD", 3, 20, &LD},
	  {0x09, "ADD", 1, 8, &UNK},
	  {0x0a, "LD", 1, 8, &LD},
	  {0x0b, "DEC", 1, 8, &DEC},
	  {0x0c, "INC", 1, 4, &INC},
	  {0x0d, "DEC", 1, 4, &DEC},
	  {0x0e, "LD", 2, 8, &LD},
	  {0x0f, "RRCA", 1, 4, &UNK},
	  {0x10, "STOP", 1, 4, &UNK},
	  {0x11, "LD", 3, 12, &LD},
	  {0x12, "LD", 1, 8, &LD},
	  {0x13, "INC", 1, 8, &INC},
	  {0x14, "INC", 1, 4, &INC},
	  {0x15, "DEC", 1, 4, &DEC},
	  {0x16, "LD", 2, 8, &LD},
	  {0x17, "RLA", 1, 4, &UNK},
	  {0x18, "JR", 2, 12, &JR},
	  {0x19, "ADD", 1, 8, &UNK},
	  {0x1a, "LD", 1, 8, &LD},
	  {0x1b, "DEC", 1, 8, &DEC},
	  {0x1c, "INC", 1, 4, &INC},
	  {0x1d, "DEC", 1, 4, &DEC},
	  {0x1e, "LD", 2, 8, &LD},
	  {0x1f, "RRA", 1, 4, &UNK},
	  {0x20, "JR", 2, 12, &JR},
	  {0x21, "LD", 3, 12, &LD},
	  {0x22, "LD", 1, 8, &LD},
	  {0x23, "INC", 1, 8, &INC},
	  {0x24, "INC", 1, 4, &INC},
	  {0x25, "DEC", 1, 4, &DEC},
	  {0x26, "LD", 2, 8, &LD},
	  {0x27, "DAA", 1, 4, &UNK},
	  {0x28, "JR", 2, 12, &JR},
	  {0x29, "ADD", 1, 8, &UNK},
	  {0x2a, "LD", 1, 8, &LD},
	  {0x2b, "DEC", 1, 8, &DEC},
	  {0x2c, "INC", 1, 4, &INC},
	  {0x2d, "DEC", 1, 4, &DEC},
	  {0x2e, "LD", 2, 8, &LD},
	  {0x2f, "CPL", 1, 4, &CPL},
	  {0x30, "JR", 2, 12, &JR},
	  {0x31, "LD", 3, 12, &LD},
	  {0x32, "LD", 1, 8, &LD},
	  {0x33, "INC", 1, 8, &INC},
	  {0x34, "INC", 1, 12, &INC},
	  {0x35, "DEC", 1, 12, &DEC},
	  {0x36, "LD", 2, 12, &LD},
	  {0x37, "SCF", 1, 4, &UNK},
	  {0x38, "JR", 2, 12, &JR},
	  {0x39, "ADD", 1, 8, &UNK},
	  {0x3a, "LD", 1, 8, &LD},
	  {0x3b, "DEC", 1, 8, &DEC},
	  {0x3c, "INC", 1, 4, &INC},
	  {0x3d, "DEC", 1, 4, &DEC},
	  {0x3e, "LD", 2, 8, &LD},
	  {0x3f, "CCF", 1, 4, &UNK},
	  {0x40, "LD", 1, 4, &LD},
	  {0x41, "LD", 1, 4, &LD},
	  {0x42, "LD", 1, 4, &LD},
	  {0x43, "LD", 1, 4, &LD},
	  {0x44, "LD", 1, 4, &LD},
	  {0x45, "LD", 1, 4, &LD},
	  {0x46, "LD", 1, 8, &LD},
	  {0x47, "LD", 1, 4, &LD},
	  {0x48, "LD", 1, 4, &LD},
	  {0x49, "LD", 1, 4, &LD},
	  {0x4a, "LD", 1, 4, &LD},
	  {0x4b, "LD", 1, 4, &LD},
	  {0x4c, "LD", 1, 4, &LD},
	  {0x4d, "LD", 1, 4, &LD},
	  {0x4e, "LD", 1, 8, &LD},
	  {0x4f, "LD", 1, 4, &LD},
	  {0x50, "LD", 1, 4, &LD},
	  {0x51, "LD", 1, 4, &LD},
	  {0x52, "LD", 1, 4, &LD},
	  {0x53, "LD", 1, 4, &LD},
	  {0x54, "LD", 1, 4, &LD},
	  {0x55, "LD", 1, 4, &LD},
	  {0x56, "LD", 1, 8, &LD},
	  {0x57, "LD", 1, 4, &LD},
	  {0x58, "LD", 1, 4, &LD},
	  {0x59, "LD", 1, 4, &LD},
	  {0x5a, "LD", 1, 4, &LD},
	  {0x5b, "LD", 1, 4, &LD},
	  {0x5c, "LD", 1, 4, &LD},
	  {0x5d, "LD", 1, 4, &LD},
	  {0x5e, "LD", 1, 8, &LD},
	  {0x5f, "LD", 1, 4, &LD},
	  {0x60, "LD", 1, 4, &LD},
	  {0x61, "LD", 1, 4, &LD},
	  {0x62, "LD", 1, 4, &LD},
	  {0x63, "LD", 1, 4, &LD},
	  {0x64, "LD", 1, 4, &LD},
	  {0x65, "LD", 1, 4, &LD},
	  {0x66, "LD", 1, 8, &LD},
	  {0x67, "LD", 1, 4, &LD},
	  {0x68, "LD", 1, 4, &LD},
	  {0x69, "LD", 1, 4, &LD},
	  {0x6a, "LD", 1, 4, &LD},
	  {0x6b, "LD", 1, 4, &LD},
	  {0x6c, "LD", 1, 4, &LD},
	  {0x6d, "LD", 1, 4, &LD},
	  {0x6e, "LD", 1, 8, &LD},
	  {0x6f, "LD", 1, 4, &LD},
	  {0x70, "LD", 1, 8, &LD},
	  {0x71, "LD", 1, 8, &LD},
	  {0x72, "LD", 1, 8, &LD},
	  {0x73, "LD", 1, 8, &LD},
	  {0x74, "LD", 1, 8, &LD},
	  {0x75, "LD", 1, 8, &LD},
	  {0x76, "HALT", 1, 4, &UNK},
	  {0x77, "LD", 1, 8, &LD},
	  {0x78, "LD", 1, 4, &LD},
	  {0x79, "LD", 1, 4, &LD},
	  {0x7a, "LD", 1, 4, &LD},
	  {0x7b, "LD", 1, 4, &LD},
	  {0x7c, "LD", 1, 4, &LD},
	  {0x7d, "LD", 1, 4, &LD},
	  {0x7e, "LD", 1, 8, &LD},
	  {0x7f, "LD", 1, 4, &LD},
	  {0x80, "ADD", 1, 4, &UNK},
	  {0x81, "ADD", 1, 4, &UNK},
	  {0x82, "ADD", 1, 4, &UNK},
	  {0x83, "ADD", 1, 4, &UNK},
	  {0x84, "ADD", 1, 4, &UNK},
	  {0x85, "ADD", 1, 4, &UNK},
	  {0x86, "ADD", 1, 8, &UNK},
	  {0x87, "ADD", 1, 4, &UNK},
	  {0x88, "ADC", 1, 4, &UNK},
	  {0x89, "ADC", 1, 4, &UNK},
	  {0x8a, "ADC", 1, 4, &UNK},
	  {0x8b, "ADC", 1, 4, &UNK},
	  {0x8c, "ADC", 1, 4, &UNK},
	  {0x8d, "ADC", 1, 4, &UNK},
	  {0x8e, "ADC", 1, 8, &UNK},
	  {0x8f, "ADC", 1, 4, &UNK},
	  {0x90, "SUB", 1, 4, &SUB},
	  {0x91, "SUB", 1, 4, &SUB},
	  {0x92, "SUB", 1, 4, &SUB},
	  {0x93, "SUB", 1, 4, &SUB},
	  {0x94, "SUB", 1, 4, &SUB},
	  {0x95, "SUB", 1, 4, &SUB},
	  {0x96, "SUB", 1, 8, &SUB},
	  {0x97, "SUB", 1, 4, &SUB},
	  {0x98, "SBC", 1, 4, &SBC},
	  {0x99, "SBC", 1, 4, &SBC},
	  {0x9a, "SBC", 1, 4, &SBC},
	  {0x9b, "SBC", 1, 4, &SBC},
	  {0x9c, "SBC", 1, 4, &SBC},
	  {0x9d, "SBC", 1, 4, &SBC},
	  {0x9e, "SBC", 1, 8, &SBC},
	  {0x9f, "SBC", 1, 4, &SBC},
	  {0xa0, "AND", 1, 4, &UNK},
	  {0xa1, "AND", 1, 4, &UNK},
	  {0xa2, "AND", 1, 4, &UNK},
	  {0xa3, "AND", 1, 4, &UNK},
	  {0xa4, "AND", 1, 4, &UNK},
	  {0xa5, "AND", 1, 4, &UNK},
	  {0xa6, "AND", 1, 8, &UNK},
	  {0xa7, "AND", 1, 4, &UNK},
	  {0xa8, "XOR", 1, 4, &XOR},
	  {0xa9, "XOR", 1, 4, &XOR},
	  {0xaa, "XOR", 1, 4, &XOR},
	  {0xab, "XOR", 1, 4, &XOR},
	  {0xac, "XOR", 1, 4, &XOR},
	  {0xad, "XOR", 1, 4, &XOR},
	  {0xae, "XOR", 1, 8, &XOR},
	  {0xaf, "XOR", 1, 4, &XOR},
	  {0xb0, "OR", 1, 4, &UNK},
	  {0xb1, "OR", 1, 4, &UNK},
	  {0xb2, "OR", 1, 4, &UNK},
	  {0xb3, "OR", 1, 4, &UNK},
	  {0xb4, "OR", 1, 4, &UNK},
	  {0xb5, "OR", 1, 4, &UNK},
	  {0xb6, "OR", 1, 8, &UNK},
	  {0xb7, "OR", 1, 4, &UNK},
	  {0xb8, "CP", 1, 4, &UNK},
	  {0xb9, "CP", 1, 4, &UNK},
	  {0xba, "CP", 1, 4, &UNK},
	  {0xbb, "CP", 1, 4, &UNK},
	  {0xbc, "CP", 1, 4, &UNK},
	  {0xbd, "CP", 1, 4, &UNK},
	  {0xbe, "CP", 1, 8, &UNK},
	  {0xbf, "CP", 1, 4, &UNK},
	  {0xc0, "RET", 1, 20, &UNK},
	  {0xc1, "POP", 1, 12, &UNK},
	  {0xc2, "JP", 3, 16, &JP},
	  {0xc3, "JP", 3, 16, &JP},
	  {0xc4, "CALL", 3, 24, &CALL},
	  {0xc5, "PUSH", 1, 16, &UNK},
	  {0xc6, "ADD", 2, 8, &UNK},
	  {0xc7, "RST", 1, 16, &UNK},
	  {0xc8, "RET", 1, 20, &UNK},
	  {0xc9, "RET", 1, 16, &UNK},
	  {0xca, "JP", 3, 16, &JP},
	  {0xcb, "PREFIX", 1, 4, &UNK},
	  {0xcc, "CALL", 3, 24, &CALL},
	  {0xcd, "CALL", 3, 24, &CALL},
	  {0xce, "ADC", 2, 8, &UNK},
	  {0xcf, "RST", 1, 16, &UNK},
	  {0xd0, "RET", 1, 20, &UNK},
	  {0xd1, "POP", 1, 12, &UNK},
	  {0xd2, "JP", 3, 16, &JP},
	  {0xd4, "CALL", 3, 24, &CALL},
	  {0xd5, "PUSH", 1, 16, &UNK},
	  {0xd6, "SUB", 2, 8, &SUB},
	  {0xd7, "RST", 1, 16, &UNK},
	  {0xd8, "RET", 1, 20, &UNK},
	  {0xd9, "RETI", 1, 16, &UNK},
	  {0xda, "JP", 3, 16, &JP},
	  {0xdc, "CALL", 3, 24, &CALL},
	  {0xde, "SBC", 2, 8, &SBC},
	  {0xdf, "RST", 1, 16, &UNK},
	  {0xe0, "LDH", 2, 12, &UNK},
	  {0xe1, "POP", 1, 12, &UNK},
	  {0xe2, "LD", 1, 8, &LD},
	  {0xe5, "PUSH", 1, 16, &UNK},
	  {0xe6, "AND", 2, 8, &UNK},
	  {0xe7, "RST", 1, 16, &UNK},
	  {0xe8, "ADD", 2, 16, &UNK},
	  {0xe9, "JP", 1, 4, &JP},
	  {0xea, "LD", 3, 16, &LD},
	  {0xee, "XOR", 2, 8, &XOR},
	  {0xef, "RST", 1, 16, &UNK},
	  {0xf0, "LDH", 2, 12, &UNK},
	  {0xf1, "POP", 1, 12, &UNK},
	  {0xf2, "LD", 1, 8, &LD},
	  {0xf3, "DI", 1, 4, &UNK},
	  {0xf5, "PUSH", 1, 16, &UNK},
	  {0xf6, "OR", 2, 8, &UNK},
	  {0xf7, "RST", 1, 16, &UNK},
	  {0xf8, "LD", 2, 12, &LD},
	  {0xf9, "LD", 1, 8, &LD},
	  {0xfa, "LD", 3, 16, &LD},
	  {0xfb, "EI", 1, 4, &UNK},
	  {0xfe, "CP", 2, 8, &UNK},
	  {0xff, "RST", 1, 16, &UNK},
};

#endif