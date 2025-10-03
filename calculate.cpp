#include <assert.h>
#include <math.h>

#include "calculate.h"

#include "processor.h"
#include "stack.h"


processor_status Calculate(Processor* processor) {
    assert(processor);
    assert(processor->byte_code_data.data);

    FILE* text = fopen(processor->byte_code_file, "r");
    if (text == NULL) {
        CHECK_ERRORS_PROC(OPEN_ERROR, perror("Error is"));
    }

    type_t comand = 0;
    type_t first_num = 0;
    type_t second_num = 0;
    type_t number = 0;
    type_t deleted_value = 0;

    while (true) {
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
                    return DIVISION_BY_ZERO;
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
                return UNKNOWN_COMAND;
        }

        if (comand == HLT) {
            break;
        }
    }

    if (fclose(text) == EOF) {
        CHECK_ERRORS_PROC(CLOSE_ERROR, perror("Error is"));
    }

    return PROC_SUCCESS;
}