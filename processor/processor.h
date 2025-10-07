#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "stdio.h"

#define CHECK_ERRORS_STACK(error, ...) \
        if (error != STACK_SUCCESS) {  \
            __VA_ARGS__;               \
            return STACK_ERROR;        \
        }

#define CHECK_ERRORS_PROC(error, ...)                  \
        if (error != PROC_SUCCESS) {                   \
            __VA_ARGS__;                               \
            /* ProcDump(processor, error, DUMP_VAR_INFO); */ \
            return error;                              \
        }

#define DO_CASE(function)            \
        CHECK_ERRORS_PROC(function); \
        break;

#define JB_SIGN <
#define JBE_SIGN <=
#define JA_SIGN >
#define JAE_SIGN >=
#define JE_SIGN ==
#define JNE_SIGN !=

#define DO_JUMP_CONDITION(sign, first_num, second_num) \
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &first_num));  \
    CHECK_ERRORS_STACK(StackPop(&processor->stack, &second_num)); \
    if (second_num sign first_num) {CHECK_ERRORS_PROC(DO_JMP(processor, pointers_data));} \
    else {processor->programm_cnt++;} \
    break;

const int CNT_REGISTERS = 8 + 1; // +1 for service


enum processor_status {
    PROC_SUCCESS                = 0,
    PROC_UNKNOWN_COMAND         = 1 << 0,
    PROC_DIVISION_BY_ZERO       = 1 << 1,
    PROC_OPEN_ERROR             = 1 << 2,
    PROC_CLOSE_ERROR            = 1 << 3,
    PROC_READ_ERROR             = 1 << 4,
    STACK_ERROR                 = 1 << 5,
    PROC_SQRT_NEGATIVE_NUM      = 1 << 6,
    PROC_STAT_ERROR             = 1 << 7,
    PROC_NOT_ENOUGH_MEMORY      = 1 << 8,
    NULL_POINTER_ON_FILE        = 1 << 9,
    NULL_POINTER_ON_BYTE_CODE   = 1 << 10,
    NULL_POINTER_ON_STRUCT_PROC = 1 << 11
};


struct Processor {
    const char* byte_code_file;
    char* byte_code_array;
    stack_t stack;
    size_t programm_cnt = 0;
    type_t registers[CNT_REGISTERS]; // Service RAX RBX RCX RDX REX RFX RGX RHX
};


int TextSize(const char *file_name);

processor_status ProcReadFile(Processor* processor);

processor_status ProcCtor(Processor* processor, const char* file_name);

processor_status ProcFillPointersArray(Processor* processor, char** pointers_data, int cnt_commands);

processor_status SPU(Processor* processor);

type_error_t ProcVerify(Processor* processor);

void ProcDump(Processor* processor, type_error_t code_error, int line, const char* function_name, const char* file_name);

processor_status ProcDtor(Processor* processor);

processor_status DO_PUSH(Processor* processor, char** pointers_data);

processor_status DO_POP(Processor* processor);

processor_status DO_ADD(Processor* processor);

processor_status DO_SUB(Processor* processor);

processor_status DO_DIV(Processor* processor);

processor_status DO_MUL(Processor* processor);

processor_status DO_SQRT(Processor* processor);

processor_status DO_POW(Processor* processor);

processor_status DO_IN(Processor* processor);

processor_status DO_POPR(Processor* processor, char** pointers_data);

processor_status DO_PUSHR(Processor* processor, char** pointers_data);

processor_status DO_OUT(Processor* processor);

processor_status DO_JMP(Processor* processor, char** pointers_data);

processor_status DO_JB(Processor* processor, char** pointers_data);


#endif //PROCESSOR_H_