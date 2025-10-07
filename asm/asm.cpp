#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

#include "canary.h"
#include "../common.h"
#include "stack.h"
#include "string_functions.h"
#include <sys/stat.h>


assembler_status AsmReadFile(Assembler* assembler) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);   

    FILE *text = fopen(assembler->comands_file, "r");
    if (text == NULL) {
        CHECK_ERRORS_ASM(ASM_OPEN_ERROR,       perror("Error is"));
    }

    assembler->about_text.text_size = TextSize(assembler->comands_file);
    if (assembler->about_text.text_size == -1) {
        CHECK_ERRORS_ASM(ASM_STAT_ERROR);
    }

    assembler->about_text.text = (char*)calloc((size_t)(assembler->about_text.text_size + 1), sizeof(char));
    if (assembler->about_text.text == NULL) {
        CHECK_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY,        perror("Error is"));
    }

    fread((char*)assembler->about_text.text, sizeof(char), (size_t)assembler->about_text.text_size, text);
    if (ferror(text) != 0) {
        CHECK_ERRORS_ASM(ASM_READ_ERROR,       fprintf(stderr, "Error is: problem with reading file"),
                                               free(assembler->about_text.text));
    }

    char* temp_text = assembler->about_text.text;

    for ( ; *temp_text != '\0'; ++temp_text) {
        if (*temp_text == '\n' || *temp_text == ' ') {
            assembler->about_text.cnt_strok++;
            // *temp_text == '\0';
        }
    }

    assembler->about_text.cnt_strok++;

    if (fclose(text) == EOF) {
        CHECK_ERRORS_ASM(ASM_CLOSE_ERROR,       perror("Error is:")); 
    }

    return ASM_SUCCESS;
}

assembler_status AsmCtor(Assembler* assembler, const char* name_comands_file, const char* name_byte_code_file) {
    assert(assembler);
    assert(name_byte_code_file);
    assert(name_comands_file);

    assembler->comands_file = name_comands_file;
    assembler->byte_code_file = name_byte_code_file;

    assembler->byte_code_data.capacity = SIZE_BYTE_CODE;
    assembler->byte_code_data.size     = 0;

    assembler->byte_code_data.data = (type_t*)calloc(assembler->byte_code_data.capacity, sizeof(type_t));

    if (assembler->byte_code_data.data == NULL){
        CHECK_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY);
    }

    AsmReadFile(assembler);

    CHECK_ERRORS_ASM(Assemblirovanie(assembler),        free(assembler->byte_code_data.data));

    CHECK_ERRORS_ASM(CreateExeFile(assembler),      free(assembler->byte_code_data.data));

    return ASM_SUCCESS;
}

int TextSize(const char *file_name) {
    assert(file_name);

    struct stat text_info = {};

    if (stat(file_name, &text_info) == -1) {
        perror("Error is");

        return -1;
    }

    return (int)text_info.st_size;
}

assembler_status AsmFillPointersArray(Assembler* assembler, char** pointers_data) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);  
    assert(assembler->about_text.text);
    assert(pointers_data);

    char* temp_text = assembler->about_text.text;
    pointers_data[0] = temp_text;

    for (int i = 1; i < assembler->about_text.cnt_strok && *temp_text != '\0'; ) {
        if (*temp_text == '\n' || *temp_text == ' ') {
            *temp_text = '\0';

            pointers_data[i] = temp_text + 1;

            ++i;
        }

        temp_text++;
    }

    return ASM_SUCCESS;
}

status_cmp FillCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file); 

    if(!strcmp(expecting_comand, (const char*)comand)) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_expecting_comand;
        assembler->byte_code_data.size++;

        return EQUAL;
    }

    return DIFFERENT;
}

assembler_status GetFillArgNum(Assembler* assembler, char* string) {
    type_t number = 0;

    if (sscanf(string, TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_NUMBER;
    }

    return ASM_SUCCESS;
}

assembler_status GetFillArgReg(Assembler* assembler, char* string) {
    char reg[LEN_REGISTER + 1] = {};

    if (sscanf(string, "%s", reg) == 1) {
        type_t code_reg = reg[1] - 'A' + 1;
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_reg;
        assembler->byte_code_data.size++;
    }
    else {
        return ASM_EXPECTS_REGISTER;
    }

    return ASM_SUCCESS;
}

void PrintfByteCode(Assembler* assembler) {
    fprintf(stderr, "Byte code:\n");

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(stderr, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }

    fprintf(stderr, "\n");
    getchar();
}

assembler_status Assemblirovanie(Assembler* assembler) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);

    char** pointers_data = (char**)calloc((size_t)(assembler->about_text.cnt_strok + 1), sizeof(char*));
    AsmFillPointersArray(assembler, pointers_data);

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        size_t old_size = assembler->byte_code_data.size;
        
        if (FillCommand(assembler, "PUSH", pointers_data[i], CMD_PUSH)) {
            CHECK_ERRORS_ASM(GetFillArgNum(assembler, pointers_data[++i]));
            continue;
        }

        if (FillCommand(assembler, "POP",   pointers_data[i], CMD_POP))  continue;
        if (FillCommand(assembler, "ADD",   pointers_data[i], CMD_ADD))  continue;
        if (FillCommand(assembler, "SUB",   pointers_data[i], CMD_SUB))  continue;
        if (FillCommand(assembler, "DIV",   pointers_data[i], CMD_DIV))  continue;
        if (FillCommand(assembler, "MUL",   pointers_data[i], CMD_MUL))  continue;
        if (FillCommand(assembler, "SQRT",  pointers_data[i], CMD_SQRT)) continue;
        if (FillCommand(assembler, "POW",   pointers_data[i], CMD_POW))  continue;
        if (FillCommand(assembler, "IN",    pointers_data[i], CMD_IN))   continue;
        if (FillCommand(assembler, "OUT",   pointers_data[i], CMD_OUT))  continue;

        if (FillCommand(assembler, "PUSHR", pointers_data[i], CMD_PUSHR)) {
            CHECK_ERRORS_ASM(GetFillArgReg(assembler, pointers_data[++i]));
            continue;
        }

        if (FillCommand(assembler, "POPR",  pointers_data[i], CMD_POPR)) {
            CHECK_ERRORS_ASM(GetFillArgReg(assembler, pointers_data[++i]));
            continue;
        }

        if (FillCommand(assembler, "JMP", pointers_data[i], CMD_JMP)) {
            CHECK_ERRORS_ASM(GetFillArgNum(assembler, pointers_data[++i]));
            continue;
        }

        if (FillCommand(assembler, "JB", pointers_data[i], CMD_JB)) {
            CHECK_ERRORS_ASM(GetFillArgNum(assembler, pointers_data[++i]));
            continue;
        }

        if (FillCommand(assembler, "HLT", pointers_data[i], CMD_HLT)) {
            break;
        }

        if (assembler->byte_code_data.size == old_size) {
            CHECK_ERRORS_ASM(ASM_UNKNOWN_COMAND,        AsmDtor(assembler); free(pointers_data));
        } 
    }


    if (assembler->byte_code_data.data[assembler->byte_code_data.size - 1] != CMD_HLT) {
        CHECK_ERRORS_ASM(ASM_EXPECTS_HLT,       AsmDtor(assembler); free(pointers_data));
    }
    
    free(pointers_data);

    PrintfByteCode(assembler);

    return ASM_SUCCESS;
}

assembler_status CreateExeFile(Assembler* assembler) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);

    FILE* text = fopen(assembler->byte_code_file, "w");
    if (text == NULL) {
        CHECK_ERRORS_ASM(ASM_OPEN_ERROR,       perror("Error is"));
    }

    fprintf(text, "%zu\n", assembler->byte_code_data.size);

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER " ", assembler->byte_code_data.data[i]);
    }

    if (fclose(text) == EOF) {
        CHECK_ERRORS_ASM(ASM_CLOSE_ERROR,      perror("Error is"));
    }

    return ASM_SUCCESS;
}

assembler_status AsmDtor(Assembler* assembler) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);

    free(assembler->byte_code_data.data);
    free(assembler->about_text.text);

    return ASM_SUCCESS;
}