#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Use pregenerated opcodes instead of manually entering them.
This makes development and change of containers easier. """

import json

def get_instructions_list(opcodes_dict):

    unique_instructions = []
    for k,v in opcodes_dict.items():
        mnemonic, length = v['mnemonic'], v['length']
        unique_instructions.append(mnemonic)
    unique_instructions = set(unique_instructions)
    return unique_instructions

def generate_signatures(unique_instructions):
    signatures = ""
    for instr in unique_instructions:
        if instr in implemented:
            signatures += "void {}(CPU &cpu, uint8_t arg1, uint8_t arg2);\n".format(instr)
    return signatures

def generate_mnemonics(opcodes_dict):
    mnemonics = ""
    for i in range(256):
        k = format(i, '#04x')
        
        # Some opcodes have no instruction attached
        if k not in opcodes_dict:
            mnemonic, length, cycles, operands = "UNK", str(0), str(0), []
            function_name = f"&{mnemonic}"
            label = mnemonic + ", ".join(operands)
            s = f"{{{k}, \"{mnemonic}\", {length}, {cycles}, {function_name}, \"{label}\"}}"
            mnemonics += f"\t{s},\n"
            continue

        v=opcodes_dict[k]
        operands=[]
        if 'operand1' in v.keys():
            operands.append(v['operand1'])
        if 'operand2' in v.keys():
            operands.append(v['operand2'])
        mnemonic, length, cycles = v['mnemonic'], str(v['length']), str(v['cycles'][0])
        function_name = f"&{mnemonic}"
        if mnemonic not in implemented:
            function_name="&UNK"
        label = mnemonic + " " + ", ".join(operands)
        s = f"{{{k}, \"{mnemonic}\", {length}, {cycles}, {function_name}, \"{label}\"}}"
        mnemonics += f"\t{s},\n"
    return mnemonics

file_template = """
    #ifndef OPCODES_H
    #define OPCODES_H

    #include <stdint.h>
    #include <string>
    #include "cpu.hpp"

    uint8_t decode(unsigned char opcode);

    void UNK(CPU &cpu, uint8_t arg1, uint8_t arg2);
    {signatures}
    struct opcodes_s {{ // The opcode value is implicit in the array index
    uint8_t opcode;
    const char *mn; // Instruction mnemonic, not including data
    uint8_t length;
    uint8_t cycles;
    void (*func)(CPU &cpu, uint8_t arg1, uint8_t arg2); // Implement minimal set of mnemonics
    //char* label;
    std::string label;
    }};

    // Array, hex opcode is array address
    const opcodes_s opcodes[] = {{
    {mnemonics}
    }};

    const opcodes_s prefixed_opcodes[] = {{
    {prefixed_mnemonics}
    }};

    #endif
    """

# Fill this list gradually as instructions are implemented in opcodes.cpp
implemented = [
    "NOP", "HALT",
    "JP", "JR",
    "LD", "LDH",
    "INC", "DEC",
    "SUB", "SBC",
    "ADD", "ADC", 
    "CPL",
    "CALL", "RET", "PUSH", "POP",
    "RETI",
    "XOR", "AND", "OR",
    "DI", "EI",
    "CP",
    "RST",
    "RLA", "RRA", "RRCA", "RLCA", "DAA", "SLA", 
    "SCF", "CCF",
    # CB-prefixed
    "RES", "BIT", "RL", "RR", "SWAP", "SRL", "RLC", "RRC", "SRA",
    "SET"
]

if __name__=="__main__":

    with open('doc/opcodes.json') as f:
        data = json.load(f)

    unique_unprefixed_instructions = get_instructions_list(data['unprefixed'])
    unique_cbprefixed_instructions = get_instructions_list(data['cbprefixed'])
    
    unique_instructions = list(unique_unprefixed_instructions) + list(unique_cbprefixed_instructions)

    signatures = generate_signatures(unique_instructions)

    
    mnemonics = generate_mnemonics(data['unprefixed'])
    prefixed_mnemonics = generate_mnemonics(data['cbprefixed'])

    # for i in range(256):
    #     k = format(i, '#04x')
        
    #     # Some opcodes have no instruction attached
    #     if k not in data['cbprefixed']:
    #         mnemonic, length, cycles = "UNK", str(0), str(0)
    #         function_name = f"&{mnemonic}"
    #         print(mnemonic)
    #         s = f"  {{{k}, \"{mnemonic}\", {length}, {cycles}, {function_name}}}"
    #         prefixed_mnemonics += f"\t{s},\n"
    #         continue

    #     v=data['cbprefixed'][k]
    #     operands = []
    #     if 'operand1' in v.keys():
    #         operands.append(v['operand1'])
    #     if 'operand2' in v.keys():
    #         operands.append(v['operand2'])
    #     # for k,v in data['unprefixed'].items():
    #     mnemonic, length, cycles = v['mnemonic'], str(v['length']), str(v['cycles'][0])
    #     label = mnemonic + " " + ", ".join(operands)
    #     function_name = f"&{mnemonic}"
    #     # print(mnemonic)
    #     if mnemonic not in implemented:
    #         function_name="&UNK"
    #     s = f"  {{{k}, \"{mnemonic}\", {length}, {cycles}, {function_name}, \"{label}\"}}"
    #     prefixed_mnemonics += f"\t{s},\n"

    args = {'signatures': signatures,
            'mnemonics': mnemonics,
            'prefixed_mnemonics': prefixed_mnemonics}

    file = file_template.format(**args)

    with open('include/opcodes.hpp', 'w') as f:
        f.write(file)

    print(f"Implemented {100*float(len(implemented))/len(unique_instructions):.1f}% of instructions ({len(implemented)} / {len(unique_instructions)})")