#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "stdio.h"

#define CHECK_AND_RETURN_ERRORS_STACK(error, ...) \
        if (error != STACK_SUCCESS) {             \
            __VA_ARGS__;                          \
            return STACK_ERROR;                   \
        }

#define CHECK_AND_RETURN_ERRORS_PROC(error, ...)       \
        if (error != PROC_SUCCESS) {                   \
            __VA_ARGS__;                               \
            fprintf(stderr, "Line %d, error %d\n", __LINE__, error);      \
            /* ProcDump(processor, error, DUMP_VAR_INFO); */ \
            return error;                              \
        }

#define DO_CASE(function)            \
        CHECK_AND_RETURN_ERRORS_PROC(function); \
        break;

#define JB_SIGN <
#define JBE_SIGN <=
#define JA_SIGN >
#define JAE_SIGN >=
#define JE_SIGN ==
#define JNE_SIGN !=

#define DO_JUMP_CONDITION(sign, first_num, second_num)                                  \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &first_num));             \
    CHECK_AND_RETURN_ERRORS_STACK(StackPop(&processor->stack, &second_num));            \
    if (second_num sign first_num) {CHECK_AND_RETURN_ERRORS_PROC(do_jmp(processor));}   \
    else {processor->programm_cnt++;}                                                   \
    break;

const int CNT_REGISTERS = 8;


enum processor_status {
    PROC_SUCCESS                  = 0,
    PROC_UNKNOWN_COMAND           = 1 << 0,
    PROC_DIVISION_BY_ZERO         = 1 << 1,
    PROC_OPEN_ERROR               = 1 << 2,
    PROC_CLOSE_ERROR              = 1 << 3,
    PROC_READ_ERROR               = 1 << 4,
    STACK_ERROR                   = 1 << 5,
    PROC_SQRT_NEGATIVE_NUM        = 1 << 6,
    PROC_STAT_ERROR               = 1 << 7,
    PROC_NOT_ENOUGH_MEMORY        = 1 << 8,
    PROC_NULL_POINTER_ON_FILE     = 1 << 9,
    PROC_NULL_POINTER_ON_DATA     = 1 << 10,
    PROC_NULL_POINTER_ON_STRUCT   = 1 << 11,
    PROC_CNT_COMMANDS_IS_NEGATIVE = 1 << 12,
    PROC_WRONG_BYTE_CODE          = 1 << 13,
    PROC_INVALID_REGISTER         = 1 << 14
};


struct Processor {
    stack_t stack;
    size_t programm_cnt = 0;
    size_t cnt_commands = 0;
    type_t registers[CNT_REGISTERS]; // RAX RBX RCX RDX REX RFX RGX RHX
    About_text about_text;
};



processor_status ProcCtor(Processor* processor, const char* file_name);

processor_status SPU(Processor* processor);

processor_status ProcVerify(Processor* processor);

void ProcDump(Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name);

processor_status ProcDtor(Processor* processor);

processor_status do_push(Processor* processor);

processor_status do_pop(Processor* processor);

processor_status do_add(Processor* processor);

processor_status do_sub(Processor* processor);

processor_status do_div(Processor* processor);

processor_status do_mul(Processor* processor);

processor_status do_sqrt(Processor* processor);

processor_status do_pow(Processor* processor);

processor_status do_in(Processor* processor);

processor_status do_popr(Processor* processor);

processor_status do_pushr(Processor* processor);

processor_status do_out(Processor* processor);

processor_status do_jmp(Processor* processor);


#endif //PROCESSOR_H_