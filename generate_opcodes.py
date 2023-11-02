""" Use pregenerated opcodes to generate our own C++ container for these """


import os, sys
# os.chdir(os.path.dirname(sys.argv[0])) # Dealing with CMAKE (https://stackoverflow.com/questions/49018868/how-do-i-make-cmake-run-a-python-script-before-building-in-order-to-generate-fi)
print(os.getcwd())
import json

with open('doc/opcodes.json') as f:
    data = json.load(f)

def get_instructions_list(opcodes_dict):

    unique_instructions = []
    for k,v in opcodes_dict.items():
        mnemonic, length = v['mnemonic'], v['length']
        # print(k, mnemonic, length)
        unique_instructions.append(mnemonic)
        # if 'operand1' in v:
            # print(v['operand1'])
    unique_instructions = set(unique_instructions)
    
    return unique_instructions

unique_instructions = get_instructions_list(data['unprefixed'])

file = """#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include "cpu.hpp"

struct opcodes_s { // The opcode value is implicit in the array index
  uint8_t opcode;
  const char *mn; // Instruction mnemonic, not including data
  uint8_t length;
  uint8_t cycles;
  void (*func)(CPU &cpu); // Implement minimal set of mnemonics
};\n
"""
implemented = [
    "NOP",
    "JP",
]

for instr in unique_instructions:
    if instr in implemented:
        file += "  void {}(CPU &cpu);\n\n".format(instr)

file += """  uint8_t decode(unsigned char opcode);

  void UNK(CPU &cpu);

// Array, hex opcode is array address
  const opcodes_s opcodes[] = {
"""

for k,v in data['unprefixed'].items():
    mnemonic, length = v['mnemonic'], str(v['length'])
    function_name = f"&{mnemonic}"
    print(mnemonic)
    if mnemonic not in implemented:
        function_name="&UNK"
    s = f"  {{{k}, \"{mnemonic}\", {length}, 4, {function_name}}}"
    file += f"\t{s},\n"
    # print(k, v['mnemonic'], v['length'])
    # break   

file += """};

#endif"""

print(file)

with open('include/opcodes.hpp', 'w') as f:
    f.write(file)