#include "opcodes.hpp"
#include <stdio.h>
#include <iostream>
#include "cpu.hpp"

void NOP(CPU &cpu){};

void UNK(CPU &cpu){
    // Placeholder for all function that have not yet been implemented
    std::cout << "Not Implemented" << std::endl;
    exit(1);

}

void JP(CPU &cpu){

}