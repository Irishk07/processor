#include "asm.h"

int main() {
    
    Assembler assembler = {};

    AsmCtor(&assembler, "asm/commands_1.txt", "processor/byte_code.txt");

    AsmDtor(&assembler);

    return 0;
}