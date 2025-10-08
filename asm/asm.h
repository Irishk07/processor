#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "../common.h"

#define CHECK_AND_RETURN_ERRORS_ASM(error, ...)        \
        if (error != ASM_SUCCESS) {                    \
            fprintf(stderr, "Error is: %d\n", error);  \
            __VA_ARGS__;                               \
            return error;                              \
        }

#define FILL_COMMAND_WITH_ARG_REG(function)                                                                    \
        if (function) {                                                                                        \
            CHECK_AND_RETURN_ERRORS_ASM(GetFillArgReg(assembler, assembler->about_text.pointer_on_text[++i])); \
            continue;                                                                                          \
        }

#define FILL_COMMAND_WITH_ARG_NUM(function)                                                                    \
        if (function) {                                                                                        \
            CHECK_AND_RETURN_ERRORS_ASM(GetFillArgNum(assembler, assembler->about_text.pointer_on_text[++i])); \
            continue;                                                                                          \
        }

#define FILL_COMMAND_WITHOUT_ARG(function) \
        if (function) {                    \
            continue;                      \
        }


const size_t SIZE_BYTE_CODE = 64;
const int LEN_NAME_REGISTER = 3;


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
    ASM_EXPECTS_REGISTER  = 1 << 8,
    ASM_NULL_POINTER_ON_STRUCT = 1 << 9,
    ASM_NULL_POINTER_ON_DATA   = 1 << 10,
    ASM_NULL_POINTER_ON_NAME_OF_FILE = 1 << 11
};

enum status_cmp {
    DIFFERENT     = 0,
    EQUAL         = 1,
    ASM_CMP_ERROR = -1
};


struct Byte_code_data {
    type_t* data;
    size_t size;
    size_t capacity;
};

struct Assembler {
    Byte_code_data byte_code_data;
    About_text about_text;
};


assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file);

assembler_status AsmVerify(Assembler* assembler);

status_cmp FillCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand);

assembler_status GetFillArgNum(Assembler* assembler, char* string);

assembler_status GetFillArgReg(Assembler* assembler, char* string);

assembler_status PrintfByteCode(Assembler* assembler);

assembler_status Assemblirovanie(Assembler* assembler);

assembler_status CreateExeFile(Assembler* assembler, const char* name_byte_code_file);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_