#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "processor.h"

#include "check_errors.h"
#include "../common.h"
#include "onegin.h"
#include "stack.h"
#include "variable_information.h"


void InitRegisters(Processor* processor) {
    for (int i = 0; i < CNT_REGISTERS; ++i) {
        processor->registers[i] = 0;
    }
}


processor_status ProcCtor(Processor* processor, const char* name_byte_code_file) {
    assert(processor);
    assert(name_byte_code_file);

    processor->about_text.text_name = name_byte_code_file;
    processor->programm_cnt = 0;
    processor->cnt_commands = 0;

    CHECK_AND_RETURN_ERRORS_STACK(STACK_CREATE(processor->stack, DEFAULT_START_CAPACITY));

    InitRegisters(processor);

    CHECK_AND_RETURN_ERRORS_PROC(OneginReadFile(processor));

    CHECK_AND_RETURN_ERRORS_PROC(OneginFillPointersArray(processor));

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

processor_status ProcVerify(Processor* processor) {
    if (processor == NULL) {
        return PROC_NULL_POINTER_ON_STRUCT;
    }

    if (processor->about_text.pointer_on_text == NULL)   return PROC_NULL_POINTER_ON_DATA;

    if (processor->about_text.text_name == NULL)         return PROC_NULL_POINTER_ON_FILE;

    if (StackVerify(&processor->stack) != STACK_SUCCESS) return STACK_ERROR;

    return PROC_SUCCESS;
}

#define DO_CASE(function)                       \
        CHECK_AND_RETURN_ERRORS_PROC(function); \
        break;

#define JB_SIGN <
#define JBE_SIGN <=
#define JA_SIGN >
#define JAE_SIGN >=
#define JE_SIGN ==
#define JNE_SIGN !=

#define DO_JUMP_CONDITION(sign, first_num, second_num)                       \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));  \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num)); \
    if (second_num sign first_num) {                                         \
        CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));                     \
    }                                                                        \
    else {processor->programm_cnt++;}                                        \
    break;

processor_status SPU(Processor* processor) {
    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    type_t command    = 0;
    type_t first_num  = 0;
    type_t second_num = 0;

    while (true) {
        if (sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &command) != 1) {
            CHECK_AND_RETURN_ERRORS_PROC(PROC_WRONG_BYTE_CODE);
        }

        switch (command) {
            case CMD_PUSH:  DO_CASE(do_push(processor));
            case CMD_POP:   DO_CASE(do_pop(processor));
            case CMD_ADD:   DO_CASE(do_add(processor));
            case CMD_SUB:   DO_CASE(do_sub(processor));
            case CMD_DIV:   DO_CASE(do_div(processor));
            case CMD_MUL:   DO_CASE(do_mul(processor));
            case CMD_SQRT:  DO_CASE(do_sqrt(processor));
            case CMD_POW:   DO_CASE(do_pow(processor));
            case CMD_IN:    DO_CASE(do_in(processor));
            case CMD_POPR:  DO_CASE(do_popr(processor));
            case CMD_PUSHR: DO_CASE(do_pushr(processor));
            case CMD_JMP:   DO_CASE(do_jmp(processor));
            case CMD_JB:    DO_JUMP_CONDITION(JB_SIGN,  first_num, second_num);
            case CMD_JBE:   DO_JUMP_CONDITION(JBE_SIGN, first_num, second_num);
            case CMD_JA:    DO_JUMP_CONDITION(JA_SIGN,  first_num, second_num);
            case CMD_JAE:   DO_JUMP_CONDITION(JAE_SIGN, first_num, second_num);
            case CMD_JE:    DO_JUMP_CONDITION(JE_SIGN,  first_num, second_num);
            case CMD_JNE:   DO_JUMP_CONDITION(JNE_SIGN, first_num, second_num);
            case CMD_OUT:   DO_CASE(do_out(processor));
            case CMD_HLT:   break;
            default:        return PROC_UNKNOWN_COMAND;
        }

        // fprintf(stderr, "Stack: ");
        // for (int i = 0; i < processor->stack.size; ++i) {
        //     fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", processor->stack.data[i]);
        // }
        // fprintf(stderr, "\n");

        if (command == CMD_HLT) {
            break;
        }

        processor->programm_cnt++;
    }

    CHECK_AND_RETURN_ERRORS_PROC(ProcVerify(processor));

    return PROC_SUCCESS;
}

#undef DO_CASE
#undef DO_JUMP_CONDITION
#undef JB_SIGN
#undef JBE_SIGN
#undef JA_SIGN
#undef JAE_SIGN
#undef JE_SIGN
#undef JNE_SIGN


void ProcDump(Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name) {
    fprintf(stderr, "Called at %s() %s:%d:\n", function_name, file_name, line);

    fprintf(stderr, "Count commands is: %zu\n", processor->cnt_commands);
    fprintf(stderr, "Now command is: %zu\n", processor->programm_cnt + 1);

    fprintf(stderr, "    registers[%d]:\n", CNT_REGISTERS);

    for (int i = 0; i < CNT_REGISTERS; ++i) {
        fprintf(stderr, "      [%d] = " TYPE_T_PRINTF_SPECIFIER " (R%cX)\n", i, processor->registers[i], i + 'A');
    }

    fprintf(stderr, "\n\n");
    fprintf(stderr, "About stack:\n");
    StackDump(&processor->stack, code_error, DUMP_VAR_INFO);
}

processor_status ProcDtor(Processor* processor) {
    assert(processor);

    processor_status code_error = ProcVerify(processor);

    StackDtor(&processor->stack);
    
    free(processor->about_text.text);
    free(processor->about_text.pointer_on_text);

    *processor = {};

    return code_error;
}


processor_status do_push(Processor* processor) {
    type_t num = 0;
    processor->programm_cnt++;

    sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num);
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_pop(Processor* processor) {
    type_t deleted_value = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &deleted_value));

    return PROC_SUCCESS;
}

processor_status do_add(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num + first_num));

    return PROC_SUCCESS;
}

processor_status do_sub(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num - first_num));

    return PROC_SUCCESS;
}

processor_status do_div(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));

    if (first_num != 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num / first_num));
    }
    else {
        return PROC_DIVISION_BY_ZERO;
    }

    return PROC_SUCCESS;
}

processor_status do_mul(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, second_num * first_num));

    return PROC_SUCCESS;
}

processor_status do_sqrt(Processor* processor) {
    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));

    if (num >= 0) {
        CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)sqrt(num)));
    }
    else {
        return PROC_SQRT_NEGATIVE_NUM;
    }

    return PROC_SUCCESS;
}

processor_status do_pow(Processor* processor) {
    type_t first_num  = 0;
    type_t second_num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, (type_t)pow(second_num, first_num)));

    return PROC_SUCCESS;
}

processor_status do_in(Processor* processor) {
    type_t num  = 0;

    scanf(TYPE_T_PRINTF_SPECIFIER, &num);
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, num));

    return PROC_SUCCESS;
}

processor_status do_popr(Processor* processor) {
    type_t num = 0;
    type_t code_reg  = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    processor->programm_cnt++;
    sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg);

    if (code_reg < 0 || code_reg >= CNT_REGISTERS) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);
    }

    processor->registers[code_reg] = num;

    return PROC_SUCCESS;
}

processor_status do_pushr(Processor* processor) {
    type_t code_reg  = 0;
    processor->programm_cnt++;

    sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &code_reg);

    if (code_reg < 0 || code_reg >= CNT_REGISTERS) {
        CHECK_AND_RETURN_ERRORS_PROC(PROC_INVALID_REGISTER);
    }
    
    CHECK_AND_RETURN_ERRORS_STACK(StackPush(&processor->stack, processor->registers[code_reg]));

    return PROC_SUCCESS;
}

processor_status do_out(Processor* processor) {
    type_t num = 0;

    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &num));
    fprintf(stderr, "Answer " TYPE_T_PRINTF_SPECIFIER "\n", num);

    return PROC_SUCCESS;
}

processor_status do_jmp(Processor* processor) {
    type_t num = 0;
    processor->programm_cnt++;

    sscanf(processor->about_text.pointer_on_text[processor->programm_cnt], TYPE_T_PRINTF_SPECIFIER, &num);
    processor->programm_cnt = (size_t)(num - 1);

    getchar();

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