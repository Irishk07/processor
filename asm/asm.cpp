#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "asm.h"

#include "../common.h"
#include "onegin.h"


void AsmInitInfoAboutCommands(Assembler* assembler) {
    assert(assembler);
    
    for (int i = 0; i < MAX_CNT_COMMANDS; ++i) {
        assembler->about_commands[i] = {};
    }

    // FIXME
    assembler->about_commands[CMD_PUSH]  = {.command_name = "PUSH",  .command_code = CMD_PUSH,  .code_of_type_argument = NUM_ARGUMENT};
    assembler->about_commands[CMD_POP]   = {.command_name = "POP",   .command_code = CMD_POP,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_ADD]   = {.command_name = "ADD",   .command_code = CMD_ADD,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_SUB]   = {.command_name = "SUB",   .command_code = CMD_SUB,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_DIV]   = {.command_name = "DIV",   .command_code = CMD_DIV,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_MUL]   = {.command_name = "MUL",   .command_code = CMD_MUL,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_SQRT]  = {.command_name = "SQRT",  .command_code = CMD_SQRT,  .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_POW]   = {.command_name = "POW",   .command_code = CMD_POW,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_IN]    = {.command_name = "IN",    .command_code = CMD_IN,    .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_OUT]   = {.command_name = "OUT",   .command_code = CMD_OUT,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_HLT]   = {.command_name = "HLT",   .command_code = CMD_HLT,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_JMP]   = {.command_name = "JMP",   .command_code = CMD_JMP,   .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JB]    = {.command_name = "JB",    .command_code = CMD_JB,    .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JBE]   = {.command_name = "JBE",   .command_code = CMD_JBE,   .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JA]    = {.command_name = "JA",    .command_code = CMD_JA,    .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JAE]   = {.command_name = "JAE",   .command_code = CMD_JAE,   .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JE]    = {.command_name = "JE",    .command_code = CMD_JE,    .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_JNE]   = {.command_name = "JNE",   .command_code = CMD_JNE,   .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_CALL]  = {.command_name = "CALL",  .command_code = CMD_CALL,  .code_of_type_argument = LABEL_ARGUMENT};
    assembler->about_commands[CMD_RET]   = {.command_name = "RET",   .command_code = CMD_RET,   .code_of_type_argument = NO_ARGUMENT};
    assembler->about_commands[CMD_PUSHR] = {.command_name = "PUSHR", .command_code = CMD_PUSHR, .code_of_type_argument = REG_ARGUMENT};
    assembler->about_commands[CMD_POPR]  = {.command_name = "POPR",  .command_code = CMD_POPR,  .code_of_type_argument = REG_ARGUMENT};
    assembler->about_commands[CMD_PUSHM] = {.command_name = "PUSHM", .command_code = CMD_PUSHM, .code_of_type_argument = RAM_REG_ARGUMENT};
    assembler->about_commands[CMD_POPM]  = {.command_name = "POPM",  .command_code = CMD_POPM,  .code_of_type_argument = RAM_REG_ARGUMENT};
}

void AsmInitLabels(Assembler* assembler) {
    for (int i = 0; i < CNT_LABELS; ++i) {
        assembler->labels[i] = -1;
    }
}

assembler_status AsmCtor(Assembler* assembler, const char* name_commands_file) {
    assert(assembler);
    assert(name_commands_file);

    assembler->about_text.text_name = name_commands_file;
    assembler->cnt_commands            = 0;

    AsmInitInfoAboutCommands(assembler);

    AsmInitLabels(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(OneginReadFile(assembler),      free(assembler->byte_code_data.data));

    CHECK_AND_RETURN_ERRORS_ASM(DivisionIntoCommands(assembler));

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, FIRST_COMPILE));

    return ASM_SUCCESS;
}


assembler_status AsmVerify(const Assembler* assembler, int number_of_compile) {
    if (assembler == NULL) {
        return ASM_NULL_POINTER_ON_STRUCT; 
    }

    if (number_of_compile == SECOND_COMPILE && assembler->byte_code_data.data == NULL) {
        return ASM_NULL_POINTER_ON_DATA;
    }

    if (assembler->about_text.text_name == NULL) {
        return ASM_NULL_POINTER_ON_NAME_OF_FILE;
    }

    return ASM_SUCCESS;
}


status_cmp FillCommand(Assembler* assembler, char* command, int index, int number_of_compile) { //FIXME Verify how realize
    assert(assembler);

    if (assembler->about_commands[index].command_name == NULL) {
        return DIFFERENT;
    }

    if(!strcmp(assembler->about_commands[index].command_name, (const char*)command)) {
        if (number_of_compile == FIRST_COMPILE) {
            assembler->cnt_commands++;
            return EQUAL;
        } 

        assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->about_commands[index].command_code;
        assembler->byte_code_data.size++;

        return EQUAL;
    }

    return DIFFERENT;
}

assembler_status GetFillArgNum(Assembler* assembler, char* string, int number_of_compile) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        if (number_of_compile == FIRST_COMPILE) {
            assembler->cnt_commands++;
            return ASM_SUCCESS;
        } 

        assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

status_cmp CheckRegister(Assembler* assembler, char* string, int type_argument) {
    if (type_argument == REG_ARGUMENT) {
        for (int i = 0; i < CNT_REGISTERS; ++i) {
            if (!strcmp(assembler->name_registers[i], string)) return EQUAL;
        }
    }

    if (type_argument == RAM_REG_ARGUMENT) {
        for (int i = 0; i < CNT_REGISTERS; ++i) { // +2 because argument has [ ]
            if (!strcmp(assembler->name_ram_registers[i], string)) return EQUAL;
        }
    }

    return DIFFERENT;
}

assembler_status GetFillArgReg(Assembler* assembler, char* string, int number_of_compile, int type_argument) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    if (!CheckRegister(assembler, string, type_argument)) {
        return ASM_EXPECTS_REGISTER;
    }

    if (number_of_compile == FIRST_COMPILE) {
        assembler->cnt_commands++;
        return ASM_SUCCESS;
    }

    type_t code_reg = 0;
    // if argument is reg, we are get first index (middle element)
    // if argument is arm_reg, we are get second index (middle element, we also have [ ])
    (type_argument == REG_ARGUMENT) ? code_reg = string[1] - 'A' : code_reg = string[2] - 'A';

    assembler->byte_code_data.data[assembler->byte_code_data.size] = code_reg;
    assembler->byte_code_data.size++;

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

assembler_status GetFillArgJump(Assembler* assembler, char* string, int number_of_compile) {
    if (GetFillArgNum(assembler, string, number_of_compile) == ASM_SUCCESS) {
        return ASM_SUCCESS;
    }

    if (*string == ':') {                                         
        int number = 0;                                                         
                                                                                
        if (sscanf(string, "%*c%d", &number) == 1) {
            if (0 <= number && number <= 9) {
                if (number_of_compile == FIRST_COMPILE) {
                    assembler->cnt_commands++;
                    return ASM_SUCCESS;
                } 

                if (number_of_compile == SECOND_COMPILE && assembler->labels[number] == -1) {
                    return ASM_NOT_FOUND_LABEL;
                }

                if (number_of_compile == SECOND_COMPILE) {
                    assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->labels[number];
                    assembler->byte_code_data.size++;
                }

                return ASM_SUCCESS;
            }

            return ASM_EXPECTS_JUMP_ARG;
        }
    }
    
    return ASM_EXPECTS_JUMP_ARG;
}


assembler_status PrintfByteCode(Assembler* assembler, int number_of_compile) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    fprintf(stderr, "Compile number %d:\n", number_of_compile);

    fprintf(stderr, "Byte code:\n");
    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Labels:\n");
    for (int i = 0; i < CNT_LABELS; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->labels[i]);
    }
    fprintf(stderr, "\n\n");

    return ASM_SUCCESS;

    // getchar(); // TODO print message
}

assembler_status Assemblirovanie(Assembler* assembler, int number_of_compile) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        int index = 0;

        if (!strcmp(assembler->about_text.pointer_on_text[i], "\0")) {
            continue;
        }

        for( ; index < MAX_CNT_COMMANDS; ++index) {
            CHECK_LABEL(assembler->about_text.pointer_on_text[i]);
            
            if (FillCommand(assembler, assembler->about_text.pointer_on_text[i], index, number_of_compile)) {

                if (assembler->about_commands[index].code_of_type_argument == NUM_ARGUMENT)   
                    CHECK_AND_RETURN_ERRORS_ASM(GetFillArgNum(assembler, assembler->about_text.pointer_on_text[++i], number_of_compile));
                    
                if (assembler->about_commands[index].code_of_type_argument == REG_ARGUMENT ||
                    assembler->about_commands[index].code_of_type_argument == RAM_REG_ARGUMENT)   
                    CHECK_AND_RETURN_ERRORS_ASM(GetFillArgReg(assembler, assembler->about_text.pointer_on_text[++i], 
                                                              number_of_compile, assembler->about_commands[index].code_of_type_argument));

                if (assembler->about_commands[index].code_of_type_argument == LABEL_ARGUMENT)
                    CHECK_AND_RETURN_ERRORS_ASM(GetFillArgJump(assembler, assembler->about_text.pointer_on_text[++i], number_of_compile));
                
                break;
            }
        }

        if (index == MAX_CNT_COMMANDS) {
            CHECK_AND_RETURN_ERRORS_ASM(ASM_UNKNOWN_COMAND);
        }
    }

    // if (number_of_compile == SECOND_COMPILE && assembler->byte_code_data.data[assembler->byte_code_data.size - 1] != CMD_HLT) { // FIXME empty lines
    //     CHECK_AND_RETURN_ERRORS_ASM(ASM_EXPECTS_HLT);
    // }
    
    PrintfByteCode(assembler, number_of_compile);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, number_of_compile));

    return ASM_SUCCESS;
}

assembler_status CreatByteCodeData(Assembler* assembler) {
    assembler->byte_code_data.capacity = (size_t)assembler->cnt_commands;
    assembler->byte_code_data.size     = 0;

    assembler->byte_code_data.data = (type_t*)calloc(assembler->byte_code_data.capacity, sizeof(type_t));

    if (assembler->byte_code_data.data == NULL){
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    return ASM_SUCCESS;
}

assembler_status CreateExeFile(const Assembler* assembler, const char* name_byte_code_file) {
    assert(name_byte_code_file);
    assert(assembler->about_text.pointer_on_text);
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler, SECOND_COMPILE));

    FILE* text = fopen(name_byte_code_file, "w");
    if (text == NULL) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_OPEN_ERROR,       perror("Error is"));
    }

    fprintf(text, "%zu\n", assembler->byte_code_data.size);

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }

    if (fclose(text) == EOF) {
        CHECK_AND_RETURN_ERRORS_ASM(ASM_CLOSE_ERROR,      perror("Error is"));
    }

    return ASM_SUCCESS;
}

assembler_status AsmDtor(Assembler* assembler) {
    assembler_status code_error = AsmVerify(assembler, SECOND_COMPILE);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);
    free(assembler->about_text.pointer_on_text);

    return code_error;
}