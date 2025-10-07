#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "stack.h"

#define CHECK_ERRORS_ASM(error, ...)              \
        if (error != ASM_SUCCESS) {               \
            fprintf(stderr, "Err: %d\n", error);  \
            __VA_ARGS__;                          \
            return error;                         \
        }

#define FILL_COMMAND_WITH_ARG_REG(function) \
        if (function) {                  \
            CHECK_ERRORS_ASM(GetFillArgReg(assembler, pointers_data[++i])); \
            continue;                    \
        }

#define FILL_COMMAND_WITH_ARG_NUM(function) \
        if (function) {                  \
            CHECK_ERRORS_ASM(GetFillArgNum(assembler, pointers_data[++i])); \
            continue;                    \
        }

const size_t SIZE_BYTE_CODE = 64;
const int LEN_REGISTER  = 3;


enum assembler_status {
    ASM_SUCCESS           = 0,
    ASM_EXPECTS_NUMBER    = 1 << 0,
    ASM_EXPECTS_HLT       = 1 << 1,
    ASM_NOT_ENOUGH_MEMORY = 1 << 2,
    ASM_UNKNOWN_COMAND    = 1 << 3,
    ASM_READ_ERROR        = 1 << 4,
    ASM_OPEN_ERROR        = 1 << 5,
    ASM_CLOSE_ERROR       = 1 << 6,
    ASM_STAT_ERROR        = 1 << 7,
    ASM_EXPECTS_REGISTER  = 1 << 8
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

assembler_status AsmFillPointersArray(Assembler* assembler, char** pointers_data);

status_cmp FillCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand);

assembler_status GetFillArgNum(Assembler* assembler, char* string);

assembler_status GetFillArgReg(Assembler* assembler, char* string);

void PrintfByteCode(Assembler* assembler);

assembler_status Assemblirovanie(Assembler* assembler);

assembler_status CreateExeFile(Assembler* assembler);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_