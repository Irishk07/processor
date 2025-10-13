#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "asm.h"

#include "../common.h"
#include "onegin.h"


void AsmInitLabels(Assembler* assembler) {
    for (int i = 0; i < CNT_LABELS; ++i) {
        assembler->labels[i] = -1;
    }
}

assembler_status AsmCtor(Assembler* assembler, const char* name_commands_file) {
    assert(assembler);
    assert(name_commands_file);

    assembler->about_text.text_name = name_commands_file;

    assembler->byte_code_data.capacity = SIZE_BYTE_CODE;
    assembler->byte_code_data.size     = 0;

    assembler->byte_code_data.data = (type_t*)calloc(assembler->byte_code_data.capacity, sizeof(type_t));

    if (assembler->byte_code_data.data == NULL){
        CHECK_AND_RETURN_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    AsmInitLabels(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    CHECK_AND_RETURN_ERRORS_ASM(OneginReadFile(assembler),      free(assembler->byte_code_data.data));

    CHECK_AND_RETURN_ERRORS_ASM(OneginFillPointersArray(assembler));

    return ASM_SUCCESS;
}


assembler_status AsmVerify(Assembler* assembler) {
    if (assembler == NULL) {
        return ASM_NULL_POINTER_ON_STRUCT; 
    }

    if (assembler->byte_code_data.data == NULL) {
        return ASM_NULL_POINTER_ON_DATA;
    }

    if (assembler->about_text.text_name == NULL) {
        return ASM_NULL_POINTER_ON_NAME_OF_FILE;
    }

    return ASM_SUCCESS;
}


status_cmp FillCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand) { //FIXME Verify how realize
    if(!strcmp(expecting_comand, (const char*)comand)) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_expecting_comand;
        assembler->byte_code_data.size++;

        return EQUAL;
    }

    return DIFFERENT;
}

assembler_status GetFillArgNum(Assembler* assembler, char* string) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status GetFillArgReg(Assembler* assembler, char* string) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    char reg[LEN_NAME_REGISTER + 1] = {};

    // 3 = LEN_NAME_REGISTER
    if (sscanf(string, "%3s", reg) == 1) {
        type_t code_reg = reg[1] - 'A';
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_reg;
        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_REGISTER;
    }

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status GetFillArgJump(Assembler* assembler, char* string) {
    if (GetFillArgNum(assembler, string) == ASM_SUCCESS) {
        return ASM_SUCCESS;
    }

    if (!strncmp(string, ":", 1)) {                                             
        int number = 0;                                                         
                                                                                
        if (sscanf(string, "%*c%d", &number) == 1) {
            if (0 <= number && number <= 9) {
                assembler->byte_code_data.data[assembler->byte_code_data.size] = assembler->labels[number];
                assembler->byte_code_data.size++;

                return ASM_SUCCESS;
            }

            return ASM_EXPECTS_JUMP_ARG;
        }
    }
    
    return ASM_EXPECTS_JUMP_ARG;
}


assembler_status PrintfByteCode(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    fprintf(stderr, "Byte code:\n");
    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Labels:\n");
    for (int i = 0; i < CNT_LABELS; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->labels[i]);
    }
    fprintf(stderr, "\n");

    return ASM_SUCCESS;

    // getchar(); // TODO print message
}

assembler_status Assemblirovanie(Assembler* assembler) {
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    assembler->byte_code_data.size = 0;

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        CHECK_LABEL(assembler->about_text.pointer_on_text[i]);

        FILL_COMMAND_WITH_ARG_NUM(FillCommand(assembler, "PUSH", assembler->about_text.pointer_on_text[i], CMD_PUSH));

        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "POP",   assembler->about_text.pointer_on_text[i], CMD_POP));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "ADD",   assembler->about_text.pointer_on_text[i], CMD_ADD));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "SUB",   assembler->about_text.pointer_on_text[i], CMD_SUB));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "DIV",   assembler->about_text.pointer_on_text[i], CMD_DIV));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "MUL",   assembler->about_text.pointer_on_text[i], CMD_MUL));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "SQRT",  assembler->about_text.pointer_on_text[i], CMD_SQRT));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "POW",   assembler->about_text.pointer_on_text[i], CMD_POW));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "IN",    assembler->about_text.pointer_on_text[i], CMD_IN));
        FILL_COMMAND_WITHOUT_ARG(FillCommand(assembler, "OUT",   assembler->about_text.pointer_on_text[i], CMD_OUT));

        FILL_COMMAND_WITH_ARG_REG(FillCommand(assembler, "PUSHR", assembler->about_text.pointer_on_text[i],  CMD_PUSHR));
        FILL_COMMAND_WITH_ARG_REG(FillCommand(assembler, "POPR",  assembler->about_text.pointer_on_text[i],  CMD_POPR));

        FILL_JUMP_COMMAND(FillCommand(assembler, "JMP", assembler->about_text.pointer_on_text[i], CMD_JMP));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JB", assembler->about_text.pointer_on_text[i],  CMD_JB));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JBE", assembler->about_text.pointer_on_text[i], CMD_JBE));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JA", assembler->about_text.pointer_on_text[i],  CMD_JA));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JAE", assembler->about_text.pointer_on_text[i], CMD_JAE));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JE", assembler->about_text.pointer_on_text[i],  CMD_JE));
        FILL_JUMP_COMMAND(FillCommand(assembler, "JNE", assembler->about_text.pointer_on_text[i], CMD_JNE));

        if (FillCommand(assembler, "HLT", assembler->about_text.pointer_on_text[i], CMD_HLT)) {
            break;
        }

        // if you here, it means that command is unknown
        CHECK_AND_RETURN_ERRORS_ASM(ASM_UNKNOWN_COMAND);
    }

    if (assembler->byte_code_data.data[assembler->byte_code_data.size - 1] != CMD_HLT) { // FIXME empty lines
        CHECK_AND_RETURN_ERRORS_ASM(ASM_EXPECTS_HLT);
    }
    
    PrintfByteCode(assembler);

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status CreateExeFile(Assembler* assembler, const char* name_byte_code_file) {
    assert(name_byte_code_file);
    assert(assembler->about_text.pointer_on_text); // FIXME
    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

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

    CHECK_AND_RETURN_ERRORS_ASM(AsmVerify(assembler));

    return ASM_SUCCESS;
}

assembler_status AsmDtor(Assembler* assembler) {
    assembler_status code_error = AsmVerify(assembler);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);
    free(assembler->about_text.pointer_on_text);

    return code_error;
}