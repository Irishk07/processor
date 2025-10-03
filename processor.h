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
        

const size_t SIZE_BYTE_CODE = 32;


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
    PROC_SUCCESS        = 0,
    EXPECTS_NUMBER      = 1 << 0,
    EXPECTS_HLT         = 1 << 1,
    NOT_ENOUGH_MEMORIES = 1 << 2,
    UNKNOWN_COMAND      = 1 << 3,
    DIVISION_BY_ZERO    = 1 << 4,
    OPEN_ERROR          = 1 << 5,
    CLOSE_ERROR         = 1 << 6,
    STACK_ERROR         = 1 << 7,
    STAT_ERROR          = 1 << 8,
    READ_ERROR          = 1 << 9
};

enum status_cmp {
    DIFFERENT  = 0,
    EQUAL = 1
};


struct About_text {
    char *text = NULL;
    const char *file_name = "comands.txt";
    int cnt_strok = 0;
    int text_size = 0;
};

struct Byte_code_data {
    type_t* data;
    size_t size;
    size_t capacity;
};

struct Processor {
    Byte_code_data byte_code_data;
    const char* byte_code_file;
    stack_t stack;
    About_text about_text;
};


processor_status ProcCtor(Processor* processor, const char* file_name);

processor_status ProcDtor(Processor* processor);


#endif //PROCESSOR_H_