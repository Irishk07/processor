#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

#include "canary.h"
#include "stack.h"
#include "string_functions.h"
#include <sys/stat.h>


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

    CHECK_ERRORS_ASM(Assemblirovanie(assembler),        free(assembler->byte_code_data.data));

    // fprintf(stderr, "%d", Assemblirovanie(assembler));

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
            *temp_text == '\0';
        }
    }

    assembler->about_text.cnt_strok++;

    if (fclose(text) == EOF) {
        CHECK_ERRORS_ASM(ASM_CLOSE_ERROR,       perror("Error is:")); 
    }

    return ASM_SUCCESS;
}

assembler_status FillPointersArray(Assembler* assembler, char** pointers_data) {
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

assembler_status DataReSize(Assembler* assembler, size_t new_capacity) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);  

    type_t* temp_data = (type_t*)my_recalloc(assembler->byte_code_data.data, new_capacity * sizeof(type_t), 
                                                                             assembler->byte_code_data.capacity * sizeof(type_t));

    if (temp_data == NULL) {
        CHECK_ERRORS_ASM(ASM_NOT_ENOUGH_MEMORY,     free(temp_data));
    }

    assembler->byte_code_data.data = temp_data;

    return ASM_SUCCESS;
}

status_cmp SetCommand(Assembler* assembler, const char* expecting_comand, char* comand, type_t code_expecting_comand) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file); 

    // fprintf(stderr, "%s\n", comand);

    if(!strcmp(expecting_comand, (const char*)comand)) {
        assembler->byte_code_data.data[assembler->byte_code_data.size] = code_expecting_comand;
        assembler->byte_code_data.size++;

        return EQUAL;
    }

    return DIFFERENT;
}

assembler_status Assemblirovanie(Assembler* assembler) {
    assert(assembler);
    assert(assembler->byte_code_data.data);
    assert(assembler->byte_code_file);
    assert(assembler->comands_file);

    AsmReadFile(assembler);
    // fprintf(stderr, "AsmReadFile %d\n", AsmReadFile(assembler));

    char** pointers_data = (char**)calloc((size_t)(assembler->about_text.cnt_strok + 1), sizeof(char*));
    FillPointersArray(assembler, pointers_data);
    //fprintf(stderr, "FillPoitersArray %d\n", FillPointersArray(assembler, pointers_data));

    for (int i = 0; i < assembler->about_text.cnt_strok; ++i) {
        size_t old_size = assembler->byte_code_data.size;

        //fprintf(stderr, "%s\n", pointers_data[i]);

        // fprintf(stderr, "%d\n", i);
        
        if (SetCommand(assembler, "PUSH", pointers_data[i], PUSH)) {
            type_t number = 0;

            i++;

            if (sscanf(pointers_data[i], TYPE_T_PRINTF_SPECIFIER, &number) == 1) {
                assembler->byte_code_data.data[assembler->byte_code_data.size] = number;
                assembler->byte_code_data.size++;
            }
            else {
                CHECK_ERRORS_ASM(ASM_EXPECTS_NUMBER,       free(assembler));
            }

            continue;
        }
        // fprintf(stderr, "%d\n", i);

        SetCommand(assembler, "POP",  pointers_data[i], POP);
        SetCommand(assembler, "ADD",  pointers_data[i], ADD);
        SetCommand(assembler, "SUB",  pointers_data[i], SUB);
        SetCommand(assembler, "DIV",  pointers_data[i], DIV);
        SetCommand(assembler, "MUL",  pointers_data[i], MUL);
        SetCommand(assembler, "SQRT", pointers_data[i], SQRT);
        SetCommand(assembler, "POW",  pointers_data[i], POW);
        SetCommand(assembler, "OUT",  pointers_data[i], OUT);

        if (SetCommand(assembler, "HLT", pointers_data[i], HLT)) {
            break;
        }

        if (assembler->byte_code_data.size == old_size) {
            fprintf(stderr, "%s\n", pointers_data[i]);
            CHECK_ERRORS_ASM(ASM_UNKNOWN_COMAND,        free(assembler->byte_code_data.data));
        } 
    }


    if (assembler->byte_code_data.data[assembler->byte_code_data.size - 1] != HLT) {
        CHECK_ERRORS_ASM(ASM_EXPECTS_HLT,       free(assembler->byte_code_data.data));
    }
    
    free(pointers_data);

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

    for (size_t i = 0; i < assembler->byte_code_data.size; ++i) {
        fprintf(text, TYPE_T_PRINTF_SPECIFIER "\n", assembler->byte_code_data.data[i]);
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