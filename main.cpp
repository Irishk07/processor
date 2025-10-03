#include <stdlib.h>

#include "asm.h"
#include "calculate.h"
#include "check_errors.h"
#include "debug.h"
#include "processor.h"
#include "stack.h"

void trash(type_t* var) {

    *(var + 8) = 134234;
}

int main() {

    // stack_t stack = {};
    // type_t var = 0;
    // type_t deleted_value = 0;

    // STACK_CREATE(stack, DEFAULT_START_CAPACITY);

    // ON_TRASH(fprintf(stderr, "var %p; stack %p", &var + 8, &stack));

    // ON_TRASH(trash(&var));
        
    // StackDtor(&stack);

    Processor processor = {};

    ProcCtor(&processor, "byte_code.txt");

    Assemblirovanie("comands.txt", &processor);

    CreateExeFile(&processor);

    Calculate(&processor);

    ProcDtor(&processor);

    return 0;
}
