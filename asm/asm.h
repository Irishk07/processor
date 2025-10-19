#ifndef ASM_H_
#define ASM_H_

#include <stdio.h>

#include "../common.h"

#define CHECK_AND_RETURN_ERRORS_ASM(error, ...)        \
        if ((error) != ASM_SUCCESS) {                  \
            fprintf(stderr, "Error is: %d, line %d\n", error, __LINE__);\
            __VA_ARGS__;                               \
            return (error);                            \
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
};

enum type_arguments {
    NO_ARGUMENT      = 0,
    NUM_ARGUMENT     = 1,
    REG_ARGUMENT     = 2,
    LABEL_ARGUMENT   = 3,
    RAM_REG_ARGUMENT = 4
};

struct About_commands {
    const char* command_name;
    type_t command_code;
    int code_of_type_argument;
    type_t argument;
};

struct Assembler {
    Byte_code_data byte_code_data;
    About_text about_text;
    type_t labels[CNT_LABELS];
    const char* const name_registers[CNT_REGISTERS] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"};
};

const About_commands about_commands [] = {
    {.command_name = "PUSH",  .command_code = CMD_PUSH,  .code_of_type_argument = NUM_ARGUMENT,     .argument = 0},
    {.command_name = "POP",   .command_code = CMD_POP,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "ADD",   .command_code = CMD_ADD,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "SUB",   .command_code = CMD_SUB,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "DIV",   .command_code = CMD_DIV,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "MUL",   .command_code = CMD_MUL,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "SQRT",  .command_code = CMD_SQRT,  .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "POW",   .command_code = CMD_POW,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "IN",    .command_code = CMD_IN,    .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "OUT",   .command_code = CMD_OUT,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "HLT",   .command_code = CMD_HLT,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "JMP",   .command_code = CMD_JMP,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JB",    .command_code = CMD_JB,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JBE",   .command_code = CMD_JBE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JA",    .command_code = CMD_JA,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JAE",   .command_code = CMD_JAE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JE",    .command_code = CMD_JE,    .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "JNE",   .command_code = CMD_JNE,   .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "CALL",  .command_code = CMD_CALL,  .code_of_type_argument = LABEL_ARGUMENT,   .argument = 0},
    {.command_name = "RET",   .command_code = CMD_RET,   .code_of_type_argument = NO_ARGUMENT,      .argument = 0},
    {.command_name = "PUSHR", .command_code = CMD_PUSHR, .code_of_type_argument = REG_ARGUMENT,     .argument = 0},
    {.command_name = "POPR",  .command_code = CMD_POPR,  .code_of_type_argument = REG_ARGUMENT,     .argument = 0},
    {.command_name = "PUSHM", .command_code = CMD_PUSHM, .code_of_type_argument = RAM_REG_ARGUMENT, .argument = 0},
    {.command_name = "POPM",  .command_code = CMD_POPM,  .code_of_type_argument = RAM_REG_ARGUMENT, .argument = 0}
};


void AsmInitLabels(Assembler* assembler);

assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file);

assembler_status AsmVerify(const Assembler* assembler, int number_of_compile);

status_cmp FindCommand(Assembler* assembler, char* string, About_commands* current_command);

void FillCommand(Assembler* assembler, About_commands* current_command, int number_of_compile);

assembler_status GetFillArgNum(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile);

status_cmp CheckRegister(Assembler* assembler, char* string, int type_argument);

assembler_status GetFillArgReg(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile, int type_argument);

assembler_status GetFillArgJump(Assembler* assembler, About_commands* current_command, char* string, int number_of_compile);

assembler_status CheckLabel(Assembler* assembler, char* string);

assembler_status PrintfByteCode(Assembler* assembler, int number_of_compile);

void FillListingFile(char* pointer_on_command, About_commands* current_command, FILE* listing_file, int type_argument);

assembler_status Assemblirovanie(Assembler* assembler, int number_of_compile);

assembler_status CreatByteCodeData(Assembler* assembler);

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file);

assembler_status AsmDtor(Assembler* assembler);


#endif //ASM_H_