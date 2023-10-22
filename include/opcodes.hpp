// Specify the instruction byte structure
// O -> Opcode
// J -> Relative jump opcode
// N -> Data byte
// d -> displacement for index addressing
// I -> Invalid opcode
#include <stdint.h>

enum OPCODE_STRUCTURE { O, OO, ON, ONN, JN, OON, OOd, OOdO, OOdN, OONN, II };

struct Z80_opcodes { // The opcode value is implicit in the array index
  unsigned int length;
  OPCODE_STRUCTURE s;
  const char *mn; // Instruction mnemonic, not including data
  uint8_t cycles;
  void (*func)();
};

void nop();
uint8_t decode(unsigned char opcode);

const Z80_opcodes opcodes[] = {
    {1, O, "NOP", 4, &nop},         // 0x00
    {3, ONN, "LD   BC, $%02x%02x"}, // 0x01
    {1, O, "LD   (BC), A"},         // 0x02
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, //Ok
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (10)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, //Ok
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (20)
    {1, O, "UNK"},
    {1, O, "UNK"}, // Ok
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (30) //Ok
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (40)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (50)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (60)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (70)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (80)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (90)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "LD E,A", 4},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (100)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (110)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (120)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (130)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "ADD A,A", 4},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (140)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (150)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (160)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (170)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (180)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (190)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {3, O, "JP nn", 12, &nop}, // 0xC3 (195)
    {3, ONN, "LD (C),A", 8},   // 0xE2 (226)
    {3, ONN, "LD A,(C)", 8},   // 0xF2 (242)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (200)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (210)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (220)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "POP HL"}, // E1
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (230)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (240)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"}, // (250)
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, O, "UNK"},
    {1, ON, "RST n", 32, &nop},
};
