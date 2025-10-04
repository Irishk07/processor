#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "processor.h"

#include "check_errors.h"
#include "stack.h"


processor_status ProcCtor(Processor* processor, const char* name_byte_code_file) {
    assert(processor);
    assert(name_byte_code_file);

    processor->byte_code_file = name_byte_code_file;

    CHECK_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY));

    //StackDump(&processor->stack, 0, DUMP_VAR_INFO);
    SPU(processor);

    return PROC_SUCCESS;
}

processor_status SPU(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_file);

    FILE* text = fopen(processor->byte_code_file, "r");
    if (text == NULL) {
        CHECK_ERRORS_PROC(PROC_OPEN_ERROR,      perror("Error is"));
    }

    type_t comand = 0;
    type_t first_num = 0;
    type_t second_num = 0;
    type_t number = 0;
    type_t deleted_value = 0;

    while (true) {
        processor->programm_cnt++;

        fscanf(text, TYPE_T_PRINTF_SPECIFIER, &comand);

        switch (comand) {
            case PUSH:
                fscanf(text, TYPE_T_PRINTF_SPECIFIER, &number);
                CHECK_ERRORS_STACK(StackPush(&processor->stack, number));
                break;
            case POP:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &deleted_value));
                break;
            case ADD:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
                CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num + first_num));
                break;
            case SUB:       
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
                CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num - first_num));
                break;
            case DIV:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));

                if (first_num != 0) {
                    CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num / first_num));
                }
                else {
                    return PROC_DIVISION_BY_ZERO;
                }
                break;
            case MUL:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
                CHECK_ERRORS_STACK(StackPush(&processor->stack, second_num * first_num));
                break;
            case SQRT:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPush(&processor->stack, (type_t)sqrt(first_num))); // FIXME
                break;
            case POW:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num));
                CHECK_ERRORS_STACK(StackPush(&processor->stack, (type_t)pow(second_num, first_num))); // FIXME
                break;
            case OUT:
                CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));
                fprintf(stderr, TYPE_T_PRINTF_SPECIFIER "\n", first_num);
                break;
            case HLT:
                break;
            default:
                return PROC_UNKNOWN_COMAND;
        }

        if (comand == HLT) {
            break;
        }
    }

    if (fclose(text) == EOF) {
        CHECK_ERRORS_PROC(PROC_CLOSE_ERROR,     perror("Error is"));
    }

    return PROC_SUCCESS;
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_file);

    StackDtor(&processor->stack);

    *processor = {};

    return PROC_SUCCESS;
}


/*

why break?

while (bytes != 0) {
    switch(bytes % 8) {
        case 0:
            memcpy(8); bytes -= 8;
            break;
        case 7:
        case 6:
        case 5:
        case 4:
            memcpy(4); bytes -= 4;
            break;
        case 3:
        case 2:
            memcpy(2); bytes -= 2;
            break;
        case 1:
            memcpy(1); bytes -= 1;
            break;
        default:
            assert(0);
    }
}

*/