#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "../common.h"

#define CHECK_AND_RETURN_ERRORS_ASM(error, ...)        \
        if ((error) != ASM_SUCCESS) {                  \
            fprintf(stderr, "Error is: %d\n", error);  \
            __VA_ARGS__;                               \
            return (error);                            \
        }

// TODO function
#define CHECK_LABEL(string)                                                     \
    if (!strncmp(string, ":", 1)) {                                             \
        int number = 0;                                                         \
                                                                                \
        if (sscanf(string, "%*c%d", &number) == 1) {                            \
            assembler->labels[number] = (type_t)assembler->byte_code_data.size; \
            break;                                                              \
        }                                                                       \
                                                                                \
        else {                                                                  \
            return ASM_NOT_FOUND_LABEL;                                           \
        }                                                                       \
    }


const int LEN_NAME_REGISTER = 3;
const int CNT_LABELS        = 10;
const int MAX_CNT_COMMANDS  = 128;
const int FIRST_COMPILE     = 1;
const int SECOND_COMPILE    = 2;


enum assembler_status {
    ASM_SUCCESS                      = 0,
    ASM_EXPECTS_NUMBER               = 1 << 0,
    ASM_EXPECTS_HLT                  = 1 << 1,
    ASM_NOT_ENOUGH_MEMORY            = 1 << 2,
    ASM_UNKNOWN_COMAND               = 1 << 3,
    ASM_READ_ERROR                   = 1 << 4,
    ASM_OPEN_ERROR                   = 1 << 5,
    ASM_CLOSE_ERROR                  = 1 << 6,
    ASM_STAT_ERROR                   = 1 << 7,
    ASM_EXPECTS_REGISTER             = 1 << 8,
    ASM_NULL_POINTER_ON_STRUCT       = 1 << 9,
    ASM_NULL_POINTER_ON_DATA         = 1 << 10,
    ASM_NULL_POINTER_ON_NAME_OF_FILE = 1 << 11,
    ASM_EXPECTS_JUMP_ARG             = 1 << 12,
    ASM_NOT_FOUND_LABEL              = 1 << 13
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

enum type_arguments {
    NO_ARGUMENT    = 0,
    NUM_ARGUMENT   = 1,
    REG_ARGUMENT   = 2,
    LABEL_ARGUMENT = 3
};

struct About_commands {
    const char* command_name;
    type_t command_code;
    int code_of_type_argument;
};

struct Assembler {
    Byte_code_data byte_code_data;
    About_text about_text;
    type_t labels[CNT_LABELS];
    About_commands about_commands[MAX_CNT_COMMANDS];
    int cnt_commands;
    const char* name_registers[CNT_REGISTERS] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"};
};


void AsmInitInfoAboutCommands(Assembler* assembler);

void AsmInitLabels(Assembler* assembler);

assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file);

assembler_status AsmVerify(const Assembler* assembler, int number_of_compile);

status_cmp FillCommand(Assembler* assembler, char* command, int index, int number_of_compile);

assembler_status GetFillArgNum(Assembler* assembler, char* string, int number_of_compile);

status_cmp CheckRegister(Assembler* assembler, char* string);

assembler_status GetFillArgReg(Assembler* assembler, char* string, int number_of_compile);

assembler_status GetFillArgJump(Assembler* assembler, char* string, int number_of_compile);

assembler_status PrintfByteCode(Assembler* assembler, int number_of_compile);

assembler_status Assemblirovanie(Assembler* assembler, int number_of_compile);

assembler_status CreatByteCodeData(Assembler* assembler);

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_