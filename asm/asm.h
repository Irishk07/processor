#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "stack.h"

#define CHECK_ERRORS_ASM(error, ...)  \
        if (error != ASM_SUCCESS) {  \
            fprintf(stderr, "Err: %d\n", error);  \
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

enum assembler_status {
    ASM_SUCCESS             = 0,
    ASM_EXPECTS_NUMBER      = 1 << 0,
    ASM_EXPECTS_HLT         = 1 << 1,
    ASM_NOT_ENOUGH_MEMORY   = 1 << 2,
    ASM_UNKNOWN_COMAND      = 1 << 3,
    ASM_READ_ERROR          = 1 << 4,
    ASM_OPEN_ERROR          = 1 << 5,
    ASM_CLOSE_ERROR         = 1 << 6,
    ASM_STAT_ERROR          = 1 << 7
};

enum status_cmp {
    DIFFERENT  = 0,
    EQUAL = 1
};


struct About_text {
    char *text = NULL;
    int cnt_strok = 0;
    int text_size = 0;
};

struct Byte_code_data {
    type_t* data;
    size_t size;
    size_t capacity;
};

struct Assembler {
    const char* comands_file = "comands.txt";
    const char* byte_code_file = "byte_code.txt";
    Byte_code_data byte_code_data;
    About_text about_text;
};


assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file, const char* name_byte_code_file);

int TextSize(const char *file_name);

assembler_status AsmReadFile(Assembler* assembler);

assembler_status FillPointersArray(Assembler* assembler, char** pointers_data);

assembler_status DataReSize(Assembler* assembler, size_t new_capacity);

status_cmp SetCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand);

assembler_status Assemblirovanie(Assembler* assembler);

assembler_status CreateExeFile(Assembler* assembler);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_