#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include "stdio.h"

#define CHECK_ERRORS_STACK(error, ...) \
        if (error != STACK_SUCCESS) {  \
            __VA_ARGS__;               \
            return STACK_ERROR;        \
        }

#define CHECK_ERRORS_PROC(error, ...) \
        if (error != PROC_SUCCESS) {  \
            __VA_ARGS__;              \
            return error;             \
        }


enum code_comand {
    PUSH = 1,
    POP  = 2,
    ADD  = 3,
    SUB  = 4,
    DIV  = 5,
    MUL  = 6,
    SQRT = 7,
    POW  = 8,
    OUT  = 9,
    HLT  = 10
};

enum processor_status {
    PROC_SUCCESS           = 0,
    PROC_UNKNOWN_COMAND    = 1 << 0,
    PROC_DIVISION_BY_ZERO  = 1 << 1,
    PROC_OPEN_ERROR        = 1 << 2,
    PROC_CLOSE_ERROR       = 1 << 3,
    READ_ERROR             = 1 << 4,
    STACK_ERROR            = 1 << 5
};


struct Processor {
    const char* byte_code_file;
    stack_t stack;
    size_t programm_cnt = 0;
};


processor_status ProcCtor(Processor* processor, const char* file_name);

processor_status SPU(Processor* processor);;

processor_status ProcDtor(Processor* processor);


#endif //PROCESSOR_H_