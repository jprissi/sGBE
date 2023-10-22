// Specify the instruction byte structure
// O -> Opcode
// J -> Relative jump opcode
// N -> Data byte
// d -> displacement for index addressing
// I -> Invalid opcode
#include <cstdint>
enum OPCODE_STRUCTURE { O, OO, ON, ONN, JN, OON, OOd, OOdO, OOdN, OONN, II };

struct Z80_opcodes { // The opcode value is implicit in the array index
  unsigned int length;
  OPCODE_STRUCTURE s;
  const char *mn; // Instruction mnemonic, not including data
  uint8_t cycles;
  void (*func)();
};

void nop();
void decode(unsigned char opcode);

const Z80_opcodes opcodes[] = {
    {1, O, "NOP", 4, &nop},         // 0x00
    {3, ONN, "LD   BC, $%02x%02x"}, // 0x01
    {1, O, "LD   (BC), A"},         // 0x02

    {3, ONN, "LD (C),A", 8}, // 0xe2 (226)
    {3, ONN, "LD A,(C)", 8}, // 0xf2 (242)

};

