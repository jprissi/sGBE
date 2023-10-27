#ifndef OPCODES_H
#define OPCODES_H

// Specify the instruction byte structure
// O -> Opcode
// J -> Relative jump opcode
// N -> Data byte
// d -> displacement for index addressing
// I -> Invalid opcode
#include <stdint.h>


// Static opcode table (human readable) -> generate dynamic opcode table (faster to run)
auto opcode_table[] = {
    'load' : {'0x15'}
}

#endif